#include "lights_s.h"
#include "scene.h"

namespace ae {

Lights_S::Lights_S(Scene *scene)
    : System{scene}
    , m_visible_lights_ssbo{BufferType::SHADER_STORAGE_BUFFER}
    , m_visible_lights_count{0}
    , m_lights_dirty{true}
{
    auto &registry = getRegistry();

    m_component_watcher.bind(registry);
    m_component_watcher.watch<Light_C, GlobalTransform_C>()
        .onCreated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onUpdated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onDestroyed([this](entt::registry &, entt::entity entity) { removeTree(entity); });

    m_visible_lights_ssbo.create(MAX_VISIBLE_LIGHTS * sizeof(GpuLight));
    m_gpu_lights.resize(MAX_VISIBLE_LIGHTS, GpuLight{});
}

void Lights_S::update()
{
    m_component_watcher.freeze();
    m_component_watcher.process();

    if (m_lights_dirty || getScene()->isCameraDirty()) {
        m_lights_dirty = false;

        // Update visible lights
        m_visible_lights_count = 0;
        m_visible_lights.clear();

        auto camera_entity = getActiveCamera();

        queryTree(m_static_lights_tree, camera_entity, m_visible_lights);
        queryTree(m_dynamic_lights_tree, camera_entity, m_visible_lights);

        m_visible_lights_count = std::min(MAX_VISIBLE_LIGHTS,
                                          static_cast<int32_t>(m_visible_lights.size()));

        if (m_visible_lights_count == 0)
            return;

        std::sort(m_visible_lights.begin(),
                  m_visible_lights.end(),
                  [](const auto &a, const auto &b) { return a.first < b.first; });

        for (int32_t i = 0; i < m_visible_lights_count; ++i)
            updateGpuLight(m_gpu_lights[i], m_visible_lights[i].second);

        m_visible_lights_ssbo.setData(m_gpu_lights.data(),
                                      m_visible_lights_count * sizeof(GpuLight));
    }
}

void Lights_S::draw(RenderState &render_state) const
{
    auto &direct_light_c = get<DirectLight_C>(getScene()->getActiveDirectLight());

    render_state.shader->uniformVec3("m_directLight.direction", direct_light_c.direction);
    render_state.shader->uniformVec4("m_directLight.ambient", direct_light_c.ambient.getColor());
    render_state.shader->uniformVec4("m_directLight.diffuse", direct_light_c.diffuse.getColor());
    render_state.shader->uniformVec4("m_directLight.specular", direct_light_c.specular.getColor());

    render_state.shader->uniformInt("u_lightsCount", m_visible_lights_count);
    Buffer::bindBase(m_visible_lights_ssbo, 1);
}

void Lights_S::clear()
{
    m_static_lights_tree.clear();
    m_dynamic_lights_tree.clear();
    m_visible_lights.clear();
    m_gpu_lights.clear();
    m_visible_lights_count = 0;
    m_lights_dirty = true;
}

float Lights_S::calculateLightRadius(entt::entity entity)
{
    const float initial_intensity = 1.0f;
    const float intensity_threshold = 0.01f;

    auto &light_c = get<Light_C>(entity);

    if (light_c.quadratic == 0.0f && light_c.linear == 0.0f)
        return 0.0f;

    // Вычисляем дискриминант
    float denominator = initial_intensity / intensity_threshold;
    float c = light_c.constant - denominator;
    float discriminant = light_c.linear * light_c.linear - 4.0f * light_c.quadratic * c;

    if (discriminant < 0.0f)
        return 0.0f;

    // Вычисляем расстояние
    float sqrt_discriminant = std::sqrt(discriminant);
    float d1 = (-light_c.linear + sqrt_discriminant) / (2.0f * light_c.quadratic);
    float d2 = (-light_c.linear - sqrt_discriminant) / (2.0f * light_c.quadratic);

    return std::max(d1, d2);
}

AABB Lights_S::calculateLightAABB(entt::entity entity) const
{
    auto &light_c = get<Light_C>(entity);

    const auto &position = getGlobalPosition(entity);
    const auto direction = getGlobalDirection(entity);

    if (light_c.type == Light_C::POINT) {
        vec3 min = position - vec3{light_c.radius};
        vec3 max = position + vec3{light_c.radius};
        return AABB{min, max};
    }

    if (light_c.type == Light_C::SPOT) {
        // Радиус основания светового конуса
        float base_radius = light_c.radius * std::tan(std::acos(light_c.outer_cut_off));

        // Центр основания конуса
        vec3 cone_base_center = position + direction * light_c.radius;

        // Ортонормированное базисное пространство вокруг направления
        vec3 up = glm::abs(direction.y) < 0.99f ? vec3{0, 1, 0} : vec3{1, 0, 0};
        vec3 right = glm::normalize(glm::cross(up, direction));
        vec3 local_up = glm::normalize(glm::cross(direction, right));

        vec3 min = position;
        vec3 max = position;

        const int segments = 6;
        for (int i = 0; i < segments; ++i) {
            float angle = glm::two_pi<float>() * (float(i) / segments);
            vec3 offset = std::cos(angle) * base_radius * right
                          + std::sin(angle) * base_radius * local_up;

            vec3 world_point = cone_base_center + offset;
            min = glm::min(min, world_point);
            max = glm::max(max, world_point);
        }

        return AABB{min, max};
    }

    return AABB{};
}

void Lights_S::updateGpuLight(GpuLight &gpu_light, entt::entity entity)
{
    const auto &light_c = get<Light_C>(entity);

    gpu_light.position = vec4(getGlobalPosition(entity), static_cast<float>(light_c.type));
    gpu_light.direction = glm::vec4(getGlobalDirection(entity),
                                    glm::cos(glm::radians(light_c.cut_off)));
    gpu_light.ambient = light_c.ambient.getColor();
    gpu_light.diffuse = light_c.diffuse.getColor();
    gpu_light.specular = light_c.specular.getColor();
    gpu_light.attenuation = vec4(light_c.constant,
                                 light_c.linear,
                                 light_c.quadratic,
                                 light_c.outer_cut_off);
}

void Lights_S::queryTree(const BVH<entt::entity, entt::null> &tree,
                         entt::entity camera_entity,
                         std::vector<std::pair<float, entt::entity> > &entities)
{
    tree.query(getCameraFrustum(camera_entity), [&](const auto &entity) {
        const auto &light_c = get<Light_C>(entity);
        if (light_c.enable) {
            float dist2 = glm::length2(getGlobalPosition(entity) - getGlobalPosition(camera_entity));
            entities.push_back({dist2, entity});
        }
    });
}

void Lights_S::updateTree(entt::entity entity)
{
    if (isValid(entity)) {
        m_lights_dirty = true;
        auto &light_c = get<Light_C>(entity);
        light_c.radius = calculateLightRadius(entity);
        auto aabb = calculateLightAABB(entity);
        has<Dynamic_C>(entity) ? m_dynamic_lights_tree.update(entity, aabb)
                               : m_static_lights_tree.update(entity, aabb);
    } else {
        removeTree(entity);
    }
}

void Lights_S::removeTree(entt::entity entity)
{
    m_lights_dirty = true;
    m_dynamic_lights_tree.remove(entity);
    m_static_lights_tree.remove(entity);
}

} // namespace ae
