#include "movement_s.h"
#include "../system/log.h"
#include "components.h"
#include "scene.h"

namespace ae {

Movement_S::Movement_S(Scene *scene)
    : System{scene}
{
    m_temp_normals.reserve(100);
    m_temp_maybe_collided.reserve(100);

    auto &registry = getRegistry();
    m_component_watcher.bind(registry);
    m_component_watcher.watch<Collider_C, GlobalTransform_C>()
        .onCreated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onUpdated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onDestroyed([this](entt::registry &, entt::entity entity) { removeTree(entity); });
}

void Movement_S::update(const Time &elapsed_time)
{
    m_component_watcher.freeze();
    m_component_watcher.process();

    auto &registry = getRegistry();

    // Двигаем объекты с коллизиями
    auto view = registry.view<Collider_C, Transform_C, Movement_C, /*InMotion_C,*/ Dynamic_C>();
    for (auto [entity, collider_c, transform_c, movement_c] : view.each()) {
        applyForces(entity, transform_c, movement_c, elapsed_time.asSeconds());

        if (glm::length2(movement_c.velocity) > 0.001f) {
            moveEntityWithCollisions(entity,
                                     collider_c,
                                     transform_c,
                                     movement_c,
                                     elapsed_time.asSeconds());
        }

        registry.patch<Transform_C>(entity);
        registry.patch<Movement_C>(entity);
    }

    // Двигаем объекты
    auto view_2 = registry.view<Movement_C, Transform_C /*, InMotion_C*/>(entt::exclude<Collider_C>);
    for (auto [entity, movement_c, transform_c] : view_2.each()) {
        if (glm::length2(movement_c.velocity) > 0.0f) {
            transform_c.position += movement_c.velocity * elapsed_time.asSeconds();
            registry.patch<Transform_C>(entity);
        }
    }
}

void Movement_S::clear()
{
    m_static_colliders_tree.clear();
    m_dynamic_colliders_tree.clear();
}

void Movement_S::updateTree(entt::entity entity)
{
    if (isValid(entity) && get<Collider_C>(entity)) {
        auto &collider_c = get<Collider_C>(entity);
        collider_c->applyTransform(getGlobalTransform(entity));
        has<Dynamic_C>(entity) ? m_dynamic_colliders_tree.update(entity, collider_c->aabb)
                               : m_static_colliders_tree.update(entity, collider_c->aabb);
    } else {
        removeTree(entity);
    }
}

void Movement_S::removeTree(entt::entity entity)
{
    m_dynamic_colliders_tree.remove(entity);
    m_static_colliders_tree.remove(entity);
}

void Movement_S::applyForces(entt::entity entity,
                             Transform_C &transform_c,
                             Movement_C &movement_c,
                             float dt)
{
    auto &vel = movement_c.velocity;

    if (movement_c.is_stepping_up) {
        movement_c.step_lerp_timer += dt;
        float t = glm::clamp(movement_c.step_lerp_timer / movement_c.step_lerp_duration, 0.0f, 1.0f);

        auto easeOutCubic = [](float t) { return 1.0f - pow(1.0f - t, 3); };

        float eased_t = easeOutCubic(t);

        // Только вертикально поднимаем, остальные силы — внизу продолжают работать
        transform_c.position.y = glm::mix(movement_c.step_start_pos.y,
                                          movement_c.step_end_pos.y,
                                          eased_t);

        if (t >= 1.0f) {
            movement_c.is_stepping_up = false;
            transform_c.position.y = movement_c.step_end_pos.y; // гарантируем финальную позицию
        }
    }

    float smoothing = 15.0f;
    float damping = 10.0f;

    vec3 current_horizontal_vel = vec3{vel.x, 0.0f, vel.z};
    vec3 new_horizontal_vel;

    if (glm::length2(movement_c.input_direction) > 0.0f) {
        vec3 desired = movement_c.input_direction * movement_c.move_speed;
        new_horizontal_vel = glm::mix(current_horizontal_vel, desired, smoothing * dt);
    } else
        new_horizontal_vel = glm::mix(current_horizontal_vel, vec3{0.0f}, damping * dt);

    if (glm::length2(new_horizontal_vel) < 0.0001f
        && glm::length2(movement_c.input_direction) < 0.0001f)
        new_horizontal_vel = vec3{0.0f};

    vec3 desired = movement_c.input_direction * movement_c.move_speed;

    vel.x = new_horizontal_vel.x;
    vel.z = new_horizontal_vel.z;

    bool grounded_and_no_input = movement_c.on_ground
                                 && glm::length2(movement_c.input_direction) < 0.0001f;

    // Гравитация
    if (!grounded_and_no_input)
        vel.y -= movement_c.gravity;
    else {
        vel.y -= movement_c.gravity;
        // Предотвращаем скатывание по наклонной поверхности
        vec3 gravity = vec3{0.0f, -movement_c.gravity, 0.0f};
        vec3 slide_vector = gravity
                            - glm::dot(gravity, movement_c.ground_normal)
                                  * movement_c.ground_normal;
        vel -= slide_vector;
    }

    // Прыжки
    if (movement_c.wants_to_jump && movement_c.on_ground) {
        vel.y = movement_c.jump_velocity;
        movement_c.on_ground = false;
        movement_c.wants_to_jump = false;
    }
}

void Movement_S::moveEntityWithCollisions(entt::entity entity,
                                          const Collider_C &collider_c,
                                          Transform_C &transform_c,
                                          Movement_C &movement_c,
                                          float dt,
                                          int32_t max_iterations)
{
    constexpr float epsilon = 0.0005f;
    constexpr float min_toi = 0.0001f;

    const auto &collider = collider_c;
    auto &vel = movement_c.velocity;
    auto &pos = transform_c.position;

    // Сбрасываем положение на земле
    movement_c.on_ground = false;

    float time_left = dt;
    vec3 delta = vec3{0.0f};
    float delta2 = 0.0f;
    AABB moved_aabb;
    AABB swept_aabb;

    m_temp_normals.clear();
    m_temp_maybe_collided.clear();

    float last_query_delta2 = -1.0f;

    for (int32_t iteration = 0; iteration < max_iterations; ++iteration) {
        if (time_left < 0.0001f)
            break;

        delta = vel * time_left;
        delta2 = glm::length2(delta);

        // Сбрасываем лучшую коллизию
        m_temp_best.reset();
        m_temp_maybe_collided.clear();

        // Только если delta сильно изменилась — пересчитываем query
        if (std::abs(delta2 - last_query_delta2) > 0.0001f) {
            moved_aabb = collider->aabb.move(pos);
            swept_aabb = moved_aabb.swept(delta);
            m_static_colliders_tree.query(swept_aabb, m_temp_maybe_collided);
            last_query_delta2 = delta2;
        }

        for (auto other : m_temp_maybe_collided) {
            auto &other_collider = get<Collider_C>(other);

            m_temp_result.reset();
            Collisions::test(collider.get(), pos, delta, other_collider.get(), m_temp_result);

            if (m_temp_result.hit) {
                if (m_temp_result.toi < m_temp_best.toi)
                    m_temp_best = m_temp_result;
            }
        }

        if (!m_temp_best.hit) {
            pos += delta;
            break;
        }

        // Попробовать шаг
        bool try_step_up = movement_c.on_ground && m_temp_best.normal.y < 0.5f
                           && glm::length2(vec3{delta.x, 0.0f, delta.z}) > 0.001f;

        if (try_step_up) {
            if (tryStepUp(entity, collider_c, transform_c, movement_c, delta, 2.5f))
                break; // Успешный шаг
        }

        // Если аабб застрял, выталкиваем его с учетом глубины, на которую он застрял
        if (m_temp_best.toi <= 0.0f) {
            pos += m_temp_best.normal * (m_temp_best.depth + epsilon);
            vel -= m_temp_best.normal * glm::dot(vel, m_temp_best.normal);
            m_temp_best.toi = 0.0f;

        } else {
            pos += delta * m_temp_best.toi + m_temp_best.normal * epsilon;
        }

        time_left -= m_temp_best.toi * time_left;

        // ⬇️ Определяем, стоит ли на земле (по направлению нормали)
        if (m_temp_best.normal.y > 0.7f) {
            movement_c.on_ground = true;
            movement_c.ground_normal = m_temp_best.normal;
        }

        bool duplicate = false;
        for (const auto &normal : m_temp_normals) {
            if (glm::dot(m_temp_best.normal, normal) > 0.99f) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && glm::dot(vel, m_temp_best.normal) < 0.1f)
            m_temp_normals.push_back(m_temp_best.normal);

        applyMultiPlaneCollisionResolution(vel, m_temp_normals);
    }

    if (!movement_c.on_ground) {
        vec3 ground_normal;
        if (raycastGroundCheck(collider->aabb, pos, 0.5f, ground_normal)) {
            movement_c.on_ground = true;
            movement_c.ground_normal = ground_normal;
        }
    }
}

bool Movement_S::tryStepUp(entt::entity entity,
                           const Collider_C &collider_c,
                           Transform_C &transform_c,
                           Movement_C &movement_c,
                           const vec3 &delta,
                           float max_step_height)
{
    const auto &collider = collider_c;
    vec3 original_pos = transform_c.position;

    constexpr int32_t step_divisions = 5; // Можно вынести в параметры
    float step_increment = max_step_height / static_cast<float>(step_divisions);

    // Один запрос — на всю область возможного движения
    m_temp_maybe_collided.clear();
    AABB top_pos_aabb = collider->aabb.move(original_pos + vec3{0.0f, max_step_height, 0.0f});
    AABB swept = top_pos_aabb.swept(delta);
    m_static_colliders_tree.query(swept, m_temp_maybe_collided);

    for (int32_t i = 1; i <= step_divisions; ++i) {
        float step_height = i * step_increment;
        vec3 step_up_pos = original_pos + vec3{0.0f, step_height, 0.0f};

        bool collision = false;

        for (auto other : m_temp_maybe_collided) {
            auto &other_collider = get<Collider_C>(other);
            m_temp_result.reset();

            Collisions::test(collider.get(), step_up_pos, delta, other_collider.get(), m_temp_result);

            if (m_temp_result.hit) {
                collision = true;
                break;
            }
        }

        if (!collision) {
            // transform_c.position = step_up_pos + delta;
            // return true;

            movement_c.is_stepping_up = true;
            movement_c.step_lerp_timer = 0.0f;
            movement_c.step_start_pos = original_pos;
            movement_c.step_end_pos = step_up_pos + delta;
            return true;
        }
    }

    // Не удалось шагнуть — откат
    transform_c.position = original_pos;
    return false;
}

bool Movement_S::raycastGroundCheck(const AABB &aabb,
                                    const vec3 &position,
                                    float max_distance,
                                    vec3 &out_normal)
{
    AABB cast_aabb = aabb.move(position);
    vec3 ray_origin = cast_aabb.getCenter();
    ray_origin.y = cast_aabb.min.y + 0.01f; // чуть выше низа коллайдера
    vec3 ray_dir = vec3{0.0f, -1.0f, 0.0f};

    Ray ray{ray_origin, ray_dir};
    RaycastResult best_hit;
    best_hit.t = max_distance;

    bool hit = false;
    std::vector<entt::entity> candidates;
    m_static_colliders_tree.query(cast_aabb.extend({0.0f, max_distance, 0.0f}), candidates);

    for (auto entity : candidates) {
        const auto &other_collider = get<Collider_C>(entity);
        RaycastResult hit_info;

        Collisions::raycast(ray, other_collider.get(), hit_info);

        if (hit_info.hit) {
            if (hit_info.t < best_hit.t && hit_info.normal.y > 0.5f) {
                best_hit = hit_info;
                hit = true;
            }
        }
    }

    if (hit) {
        out_normal = best_hit.normal;
        return true;
    }

    return false;
}

void Movement_S::clipVelocity(const vec3 &in, const vec3 &normal, vec3 &out, float overbounce) const
{
    float backoff = glm::dot(in, normal);
    if (backoff < 0.0f)
        backoff *= overbounce;
    else
        backoff /= overbounce;

    out = in - normal * backoff;
}

void Movement_S::applyMultiPlaneCollisionResolution(vec3 &velocity,
                                                    const std::vector<vec3> &normals) const
{
    constexpr float epsilon = 0.001f;

    if (normals.empty())
        return;

    if (normals.size() == 1) {
        // Один контакт — обычный клип
        clipVelocity(velocity, normals[0], velocity, 1.001f);
        return;
    }

    if (normals.size() == 2) {
        // Два контакта — скольжение по ребру
        vec3 dir = glm::cross(normals[0], normals[1]);

        if (glm::length2(dir) < 1e-6f) {
            // Почти параллельные нормали — клип по одной (первой)
            clipVelocity(velocity, normals[0], velocity, 1.001f);
        } else {
            dir = glm::normalize(dir);
            float d = glm::dot(velocity, dir);
            velocity = dir * d;
        }

        return;
    }

    // Три и более — скорее всего тупик, обнуляем
    velocity = vec3{0.0f};
}

} // namespace ae
