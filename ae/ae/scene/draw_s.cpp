#include "draw_s.h"
#include "../engine.h"
#include "scene.h"

#include <execution>

namespace ae {

Draw_S::Draw_S(Scene *scene)
    : System{scene}
    , m_draw_dirty{true}
{
    auto &registry = getRegistry();

    m_component_watcher.bind(registry);
    m_component_watcher.watch<Drawable_C, GlobalTransform_C>()
        .onCreated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onUpdated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onDestroyed([this](entt::registry &, entt::entity entity) { removeTree(entity); });
}

void Draw_S::update()
{
    m_component_watcher.freeze();
    m_component_watcher.process();

    if (isSceneDirty() || isCameraDirty()) {
        const auto &camera_c = get<Camera_C>(getActiveCamera());

        // Update visible entities
        m_visible_entities.clear();
        m_visible_transparent_entities.clear();

        auto camera_entity = getActiveCamera();

        queryTree(m_static_draw_tree,
                  camera_entity,
                  m_visible_entities,
                  m_visible_transparent_entities);
        queryTree(m_dynamic_draw_tree,
                  camera_entity,
                  m_visible_entities,
                  m_visible_transparent_entities);

        std::sort(std::execution::par_unseq,
                  m_visible_transparent_entities.begin(),
                  m_visible_transparent_entities.end(),
                  [](const auto &a, const auto &b) { return a.first > b.first; });
    }
}

void Draw_S::drawEntities(RenderState &render_state) const
{
    auto &registry = getRegistry();

    // debugDraw(registry, render_state);

    drawEntities(registry, m_visible_entities, render_state);
    drawEntities(registry, m_visible_transparent_entities, render_state);
}

void Draw_S::clear()
{
    m_static_draw_tree.clear();
    m_dynamic_draw_tree.clear();

    m_visible_entities.clear();
    m_visible_transparent_entities.clear();

    m_draw_dirty = true;
}

void Draw_S::drawEntities(const entt::registry &registry,
                          const std::vector<std::pair<float, entt::entity> > &entities,
                          RenderState &render_state) const
{
    for (const auto &[_, entity] : entities) {
        if (!registry.valid(entity))
            continue;

        render_state.transform = getGlobalTransform(entity);
        render_state.shader->uniformInt("u_skeleton", false);
        render_state.shader->uniformInt("u_enableLight", true);

        auto &drawable_c = registry.get<Drawable_C>(entity);
        if (drawable_c)
            drawable_c->draw(render_state);
    }
}

void Draw_S::debugDraw(const entt::registry &registry, RenderState &render_state) const
{
    // render velocity
    render_state.shader->uniformInt("u_enableLight", false);
    render_state.shader->uniformInt("u_skeleton", false);

    auto movement_view = registry.view<Movement_C, InMotion_C>();
    movement_view.each([&](auto entity, auto &movement_c) {
        drawLine(getGlobalPosition(entity),
                 getGlobalPosition(entity) + movement_c.velocity,
                 Color::green,
                 render_state);
    });

    // render colliders aabb
    auto collider_view = registry.view<Collider_C>();
    collider_view.each([&](auto entity, auto &collider_c) {
        drawDrawableAABB(collider_c->aabb, Color::blue, render_state);
    });

    // render collisions
    auto collisions_view = registry.view<Collisions_C>();
    collisions_view.each([&](auto entity, auto &collisions_c) {
        auto movement_c = get<Movement_C>(entity);
        for (auto &col : collisions_c.collisions) {
            if (glm::length(col.normal) > 0.0f)
                drawLine(col.point, col.point + col.normal, Color::white, render_state);

            drawDrawableAABB(col.aabb,
                             Color{1.0f * col.toi, 0.0f, 1.0f * col.toi, 1.0f},
                             render_state);

            if (glm::length(col.vel) > 0.0f)
                drawLine(col.aabb.getCenter(),
                         col.aabb.getCenter() + col.vel,
                         Color{1.0f, 0.5f, 0.5f, 1.0f},
                         render_state);

            drawTriangle(col.v0, col.v1, col.v2, Color{1.0f, 1.0f, 0.0f, 1.0f}, render_state);
        }
    });

    debugDrawEntities(registry, m_visible_entities, render_state);
    debugDrawEntities(registry, m_visible_transparent_entities, render_state);
}

void Draw_S::debugDrawEntities(const entt::registry &registry,
                               const std::vector<std::pair<float, entt::entity> > &entities,
                               RenderState &render_state) const
{
    for (const auto &[_, entity] : entities) {
        if (!registry.valid(entity))
            continue;

        render_state.transform = getGlobalTransform(entity);

        auto &drawable_c = registry.get<Drawable_C>(entity);
        if (drawable_c)
            drawDrawableAABB(getGlobalAABB(entity), Color::red, render_state);
    }
}

void Draw_S::drawDrawableAABB(const AABB &aabb,
                              const Color &color,
                              const RenderState &render_state) const
{
    const vec3 &min = aabb.min;
    const vec3 &max = aabb.max;

    std::vector<vec3> corners = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z},
    };

    std::vector<uint32_t> indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                     6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

    std::vector<Vertex> line_vertices;
    for (const auto &corner : corners)
        line_vertices.push_back({corner, vec3{0.0f}, color.getColor(), vec2{0.0f}});

    VertexArray line_array;
    line_array.create(line_vertices, indices);

    glLineWidth(3.0f);

    Texture::bind(*Texture::getDefaultDiffuseTexture());

    render_state.shader->uniformMatrix("u_model", mat4{1.0f});
    render_state.shader->uniformVec4("u_material.color", color.getColor());
    render_state.shader->uniformFloat("u_material.shininess", 0.0f);

    line_array.draw(PrimitiveType::LINES);

    Texture::unbind();
}

void Draw_S::drawLine(const vec3 &p1,
                      const vec3 &p2,
                      const Color &color,
                      const RenderState &render_state) const
{
    std::vector<Vertex> vertices;
    vertices.push_back({p1, glm::vec3(0.0f), color.getColor(), glm::vec2(0.0f)});
    vertices.push_back({p2, glm::vec3(0.0f), color.getColor(), glm::vec2(0.0f)});

    std::vector<uint32_t> indices(vertices.size());
    for (uint32_t i = 0; i < indices.size(); ++i)
        indices[i] = i;

    VertexArray line_array;
    line_array.create(vertices, indices);

    glLineWidth(3.0f);

    Texture::bind(*Texture::getDefaultDiffuseTexture());

    render_state.shader->uniformMatrix("u_model", mat4{1.0f});
    render_state.shader->uniformVec4("u_material.color", color.getColor());
    render_state.shader->uniformFloat("u_material.shininess", 0.0f);

    line_array.draw(PrimitiveType::LINES);

    Texture::unbind();
}

void Draw_S::drawTriangle(const vec3 &v0,
                          const vec3 &v1,
                          const vec3 &v2,
                          const Color &color,
                          const RenderState &render_state) const
{
    std::vector<Vertex> vertices;
    vertices.push_back({v0, glm::vec3(0.0f), color.getColor(), glm::vec2(0.0f)});
    vertices.push_back({v1, glm::vec3(0.0f), color.getColor(), glm::vec2(0.0f)});
    vertices.push_back({v2, glm::vec3(0.0f), color.getColor(), glm::vec2(0.0f)});

    std::vector<uint32_t> indices = {2, 1, 0};

    VertexArray vertex_array;
    vertex_array.create(vertices, indices);

    glLineWidth(3.0f);

    Texture::bind(*Texture::getDefaultDiffuseTexture());

    render_state.shader->uniformMatrix("u_model", mat4{1.0f});
    render_state.shader->uniformVec4("u_material.color", color.getColor());
    render_state.shader->uniformFloat("u_material.shininess", 0.0f);

    vertex_array.draw(PrimitiveType::TRIANGLES, false);

    Texture::unbind();
}

void Draw_S::queryTree(const BVH<entt::entity, entt::null> &tree,
                       entt::entity camera_entity,
                       std::vector<std::pair<float, entt::entity> > &entities,
                       std::vector<std::pair<float, entt::entity> > &transparent_entities)
{
    tree.query(getCameraFrustum(camera_entity), [&](const auto &entity) {
        if (has<Drawable_C>(entity)) {
            auto &drawable_c = get<Drawable_C>(entity);

            if (!drawable_c)
                return;

            if (drawable_c->isTransparent()) {
                float dist2 = glm::length2(getGlobalPosition(entity)
                                           - getGlobalPosition(camera_entity));
                transparent_entities.push_back({dist2, entity});
            } else
                entities.push_back({0.0f, entity});
        }
    });
}

void Draw_S::updateTree(entt::entity entity)
{
    if (isValid(entity) && get<Drawable_C>(entity)) {
        m_draw_dirty = true;
        const auto &aabb = getGlobalAABB(entity);
        has<Dynamic_C>(entity) ? m_dynamic_draw_tree.update(entity, aabb)
                               : m_static_draw_tree.update(entity, aabb);
    } else
        removeTree(entity);
}

void Draw_S::removeTree(entt::entity entity)
{
    m_draw_dirty = true;
    m_static_draw_tree.remove(entity);
    m_dynamic_draw_tree.remove(entity);
}

} // namespace ae
