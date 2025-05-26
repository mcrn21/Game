#ifndef AE_LIGHTS_S_H
#define AE_LIGHTS_S_H

#include "../graphics/core/buffer.h"
#include "../graphics/core/render_state.h"
#include "bvh.h"
#include "components.h"
#include "multi_component_watcher.h"
#include "system.h"

#include <entt/entt.hpp>

#define MAX_VISIBLE_LIGHTS 8

namespace ae {

class Lights_S : public System
{
public:
    Lights_S(Scene *scene);
    ~Lights_S() = default;

    void update();
    void draw(RenderState &render_state) const;

private:
    struct GpuLight
    {
        vec4 position;  // .w = type (0 = point, 1 = spot)
        vec4 direction; // .w = cutOff (in radians or cos value)
        vec4 ambient;
        vec4 diffuse;
        vec4 specular;
        vec4 attenuation; // x = constant, y = linear, z = quadratic, w = radius or padding
    };

    float calculateLightRadius(entt::entity entity);
    AABB calculateLightAABB(entt::entity entity) const;
    void updateGpuLight(GpuLight &gpu_light, entt::entity entity);

    void queryTree(const BVH<entt::entity, entt::null> &tree,
                   entt::entity camera_entity,
                   std::vector<std::pair<float, entt::entity>> &entities);
    void updateTree(entt::entity entity);
    void removeTree(entt::entity entity);

private:
    BVH<entt::entity, entt::null> m_static_lights_tree;
    BVH<entt::entity, entt::null> m_dynamic_lights_tree;
    ComponentWatcher m_component_watcher;
    std::vector<std::pair<float, entt::entity>> m_visible_lights;
    Buffer m_visible_lights_ssbo;
    std::vector<GpuLight> m_gpu_lights;
    int32_t m_visible_lights_count;
    bool m_lights_dirty;
};

} // namespace ae

#endif // AE_LIGHTS_S_H
