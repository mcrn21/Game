#ifndef AE_COLLISIONS_S_H
#define AE_COLLISIONS_S_H

#include "../collisions/colliders.h"
#include "../system/time.h"
#include "bvh.h"
#include "multi_component_watcher.h"
#include "system.h"

#include <entt/entt.hpp>

namespace ae {

class Scene;

class Collisions_S : public System
{
public:
    Collisions_S(Scene *scene);
    ~Collisions_S() = default;

    void update(const Time &elapsed_time);

private:
    void resolveCollisions(entt::entity entity,
                           const std::shared_ptr<Collider> &collider,
                           vec3 &position,
                           vec3 &velocity,
                           float dt,
                           int32_t max_iterations = 4);

    // void queryTree(entt::registry &registry,
    //                const BVH<entt::entity, entt::null> &tree,
    //                const Camera_C &camera_c,
    //                std::vector<std::pair<float, entt::entity>> &entities);
    void updateTree(entt::entity entity);
    void removeTree(entt::entity entity);

private:
    BVH<entt::entity, entt::null> m_static_colliders_tree;
    BVH<entt::entity, entt::null> m_dynamic_colliders_tree;
    ComponentWatcher m_component_watcher;

    bool m_debug;
};

} // namespace ae

#endif // AE_COLLISIONS_S_H
