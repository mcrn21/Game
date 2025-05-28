#include "collisions_s.h"
#include "../app.h"
#include "../collisions/collisions.h"
#include "../common/spdlog_utils.h"
#include "../graphics/scene/shape.h"
#include "components.h"
#include "scene.h"

#include <entt/entt.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <spdlog/spdlog.h>

namespace ae {

void PM_ClipVelocity(const vec3 &in, const vec3 &normal, vec3 &out, float overbounce)
{
    float backoff = glm::dot(in, normal);
    if (backoff < 0.0f)
        backoff *= overbounce;
    else
        backoff /= overbounce;

    out = in - normal * backoff;
}

void ApplyMultiPlaneCollisionResolution(vec3 &vel, const std::vector<CollisionResult> &normals)
{
    constexpr float epsilon = 0.001f;
    constexpr float stop_threshold = 0.001f;

    if (normals.empty())
        return;

    if (normals.size() == 1) {
        // Один контакт — обычный клип
        PM_ClipVelocity(vel, normals[0].normal, vel, 1.001f);
        return;
    }

    if (normals.size() == 2) {
        // Два контакта — скольжение по ребру
        vec3 dir = glm::cross(normals[0].normal, normals[1].normal);

        if (glm::length2(dir) < 1e-6f) {
            // Почти параллельные нормали — клип по одной (первой)
            PM_ClipVelocity(vel, normals[0].normal, vel, 1.001f);
        } else {
            dir = glm::normalize(dir);
            float d = glm::dot(vel, dir);
            vel = dir * d;
        }

        return;
    }

    // Три и более — скорее всего тупик, обнуляем
    vel = vec3(0.0f);
}

Collisions_S::Collisions_S(Scene *scene)
    : System{scene}
    , m_debug{false}
{
    auto &registry = getRegistry();

    m_component_watcher.bind(registry);
    m_component_watcher.watch<Collider_C, GlobalTransform_C>()
        .onCreated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onUpdated([this](entt::registry &, entt::entity entity) { updateTree(entity); })
        .onDestroyed([this](entt::registry &, entt::entity entity) { removeTree(entity); });
}

void Collisions_S::update(const Time &elapsed_time)
{
    auto &registry = getRegistry();

    m_component_watcher.freeze();
    m_component_watcher.process();

    if (App::getInstance().getInput()->isKeyPressed(KeyCode::C)) {
        m_debug = !m_debug;
    }

    auto view = registry.view<Collider_C, Transform_C, Movement_C, InMotion_C, Dynamic_C>();
    for (auto [entity, collider_c, transform_c, movement_c] : view.each()) {
        vec3 &position = transform_c.position;
        vec3 &velocity = movement_c.velocity;

        resolveCollisions(entity, collider_c, position, velocity, elapsed_time.asSeconds(), 4);

        registry.patch<Transform_C>(entity);
        registry.patch<Movement_C>(entity);
    }
}

void Collisions_S::resolveCollisions(entt::entity entity,
                                     const SharedPtr<Collider> &collider,
                                     vec3 &position,
                                     vec3 &velocity,
                                     float dt,
                                     int32_t max_iterations)
{
    vec3 &pos = position;
    vec3 &vel = velocity;
    float time_left = dt;

    // vec3 pos = position;
    // vec3 vel = glm::normalize(velocity) * 5.0f;
    // float time_left = 1.0f; //dt; //1.0f;

    // vec3 pos = position;
    // vec3 vel = velocity;
    // float time_left = dt;

    vec3 delta = vel * time_left;

    constexpr float epsilon = 0.0005f;
    constexpr float min_toi = 0.0001f;

    // Для дебажной отрисовки
    auto &collisions_c = getRegistry().get_or_emplace<Collisions_C>(entity);

    CollisionResult result;
    CollisionResult best;
    std::vector<CollisionResult> collision_results;
    std::vector<CollisionResult> hits;
    std::vector<entt::entity> maybe_collided;

    AABB moved_aabb = collider->aabb.move(pos);
    AABB swept_aabb = moved_aabb.swept(vel * time_left);

    bool check = false;

    // for (int32_t iteration = 0; iteration < max_iterations; ++iteration) {
    //     moved_aabb = collider->aabb.move(pos);

    //     best.reset();
    //     maybe_collided.clear();
    //     m_static_colliders_tree.query(moved_aabb, maybe_collided);

    //     for (auto other : maybe_collided) {
    //         auto &other_collider = get<Collider_C>(other);

    //         result.reset();
    //         Collisions::test(collider.get(), pos, other_collider.get(), result);

    //         if (result.hit) {
    //             if (result.depth < best.depth)
    //                 best = result;
    //         }
    //     }

    //     if (!best.hit)
    //         break;

    //     pos += best.normal * (best.depth + epsilon);
    //     vel -= best.normal * glm::dot(vel, best.normal);
    // }

    // return;

    for (int32_t iteration = 0; iteration < max_iterations; ++iteration) {
        spdlog::debug("iteration: {}", iteration);
        delta = vel * time_left;

        // Сбрасываем лучшую коллизию
        best.reset();
        maybe_collided.clear();
        hits.clear();

        moved_aabb = collider->aabb.move(pos);
        swept_aabb = moved_aabb.swept(vel * time_left);

        m_static_colliders_tree.query(swept_aabb, maybe_collided);

        for (auto other : maybe_collided) {
            auto &other_collider = get<Collider_C>(other);

            result.reset();
            Collisions::test(collider.get(), pos, delta, other_collider.get(), result);

            if (result.hit) {
                spdlog::debug("hit: normal: {}, depth: {}", result.normal, result.depth);
                hits.push_back(result);
                if (result.toi < best.toi)
                    best = result;

                // if (result.depth < best.depth)
                //     best = result;
            }
        }

        if (!best.hit) {
            if (iteration > 0)
                check = true;
            break;
        }

        // Для дебажной отрисовки
        if (iteration == 0) {
            collisions_c.collisions.clear();
            // Collisions_C::Collision c;
            // c.point = pos;
            // c.aabb = collider->aabb.move(pos);
            // collisions_c.collisions.push_back(c);
        }

        spdlog::debug("toi: {}, Collided: {}, velocity: {}",
                      best.toi,
                      best.normal,
                      glm::length(delta));

        // Если аабб застрял, выталкиваем его с учетом глубины, на которую он застрял
        if (best.toi <= 0.0f) {
            // continue;
            // check = true;
            // break;

            // for (auto &h : hits) {
            //     if (h.toi == 0.0f /*glm::dot(h.normal, delta) < 0.1f*/)
            //         pos += h.normal * h.depth + epsilon;
            // }

            // spdlog::debug("normal: {}, depth: {}", best.normal, best.depth);
            // // continue;
            // pos += best.normal * (best.depth + epsilon);
            // vel -= best.normal * glm::dot(vel, best.normal);
            // best.toi = 0.0f; //min_toi;

        } else {
            pos += delta * best.toi + best.normal * epsilon;
        }

        time_left -= best.toi * time_left;

        bool duplicate = false;
        for (const auto &r : collision_results) {
            if (glm::dot(best.normal, r.normal) > 0.99f) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && glm::dot(vel, best.normal) < 0.1f) {
            spdlog::debug("Add normal");
            collision_results.push_back(best);
        }

        // Для дебажной отрисовки
        {
            Collisions_C::Collision c;
            c.normal = best.normal;
            c.v0 = best.v0;
            c.v1 = best.v1;
            c.v2 = best.v2;
            c.toi = best.toi;
            c.aabb = collider->aabb.move(best.point);
            c.point = best.point;
            collisions_c.collisions.push_back(c);

            // auto &prev = collisions_c.collisions[collisions_c.collisions.size() - 2];
            // prev.vel = delta * best.toi;
        }

        ApplyMultiPlaneCollisionResolution(vel, collision_results);
    }

    if (check) {
        spdlog::debug("check");

        for (int32_t iteration = 0; iteration < max_iterations; ++iteration) {
            moved_aabb = collider->aabb.move(pos);

            best.reset();
            maybe_collided.clear();
            m_static_colliders_tree.query(moved_aabb, maybe_collided);

            for (auto other : maybe_collided) {
                auto &other_collider = get<Collider_C>(other);

                result.reset();
                Collisions::test(collider.get(), pos, other_collider.get(), result);

                if (result.hit) {
                    if (result.depth < best.depth)
                        best = result;
                }
            }

            if (!best.hit)
                break;

            pos += best.normal * (best.depth + epsilon);
        }

        for (int32_t iteration = 0; iteration < max_iterations; ++iteration) {
            delta = vel * dt;
            moved_aabb = collider->aabb.move(pos + delta);

            best.reset();
            maybe_collided.clear();
            m_static_colliders_tree.query(moved_aabb, maybe_collided);

            for (auto other : maybe_collided) {
                auto &other_collider = get<Collider_C>(other);

                result.reset();
                Collisions::test(collider.get(), pos + delta, other_collider.get(), result);

                if (result.hit) {
                    if (result.depth < best.depth)
                        best = result;
                }
            }

            if (!best.hit)
                break;

            vel -= best.normal * glm::dot(vel, best.normal);
        }
    }
}

void Collisions_S::updateTree(entt::entity entity)
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

void Collisions_S::removeTree(entt::entity entity)
{
    m_static_colliders_tree.remove(entity);
    m_dynamic_colliders_tree.remove(entity);
}

} // namespace ae
