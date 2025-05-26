#ifndef AE_MOVEMENT_S_H
#define AE_MOVEMENT_S_H

#include "../collisions/collisions.h"
#include "../system/time.h"
#include "components.h"
#include "multi_component_watcher.h"
#include "system.h"

#include <entt/entt.hpp>

namespace ae {

class Scene;

class Movement_S : public System
{
public:
    Movement_S(Scene *scene);
    ~Movement_S() = default;

    void update(const Time &elapsed_time);

private:
    void updateTree(entt::entity entity);
    void removeTree(entt::entity entity);

    void applyForces(entt::entity entity,
                     Transform_C &transform_c,
                     Movement_C &movement_c,
                     float dt);

    void moveEntityWithCollisions(entt::entity entity,
                                  const Collider_C &collider_c,
                                  Transform_C &transform_c,
                                  Movement_C &movement_c,
                                  float dt,
                                  int32_t max_iterations = 4);

    bool tryStepUp(entt::entity entity,
                   const Collider_C &collider_c,
                   Transform_C &transform_c,
                   Movement_C &movement_c,
                   const vec3 &delta,
                   float max_step_height);

    bool raycastGroundCheck(const AABB &aabb,
                            const vec3 &position,
                            float max_distance,
                            vec3 &out_normal);

    void clipVelocity(const vec3 &in, const vec3 &normal, vec3 &out, float overbounce) const;
    void applyMultiPlaneCollisionResolution(vec3 &velocity, const std::vector<vec3> &normals) const;

private:
    ComponentWatcher m_component_watcher;

    // Временные переменные для поиска коллизий
    CollisionResult m_temp_result;
    CollisionResult m_temp_best;
    std::vector<vec3> m_temp_normals;
    std::vector<entt::entity> m_temp_maybe_collided;
};

} // namespace ae

#endif // AE_MOVEMENT_S_H
