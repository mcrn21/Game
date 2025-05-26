#ifndef AE_DRAW_S_H
#define AE_DRAW_S_H

#include "../graphics/core/render_state.h"
#include "bvh.h"
#include "multi_component_watcher.h"
#include "components.h"
#include "system.h"

#include <entt/entt.hpp>

namespace ae {

class Draw_S : public System
{
public:
    Draw_S(Scene *scene);
    ~Draw_S() = default;

    void update();
    void drawEntities(RenderState &render_state) const;

private:
    void drawEntities(const entt::registry &registry,
                      const std::vector<std::pair<float, entt::entity>> &entities,
                      RenderState &render_state) const;

    void debugDraw(const entt::registry &registry, RenderState &render_state) const;
    void debugDrawEntities(const entt::registry &registry,
                           const std::vector<std::pair<float, entt::entity>> &entities,
                           RenderState &render_state) const;

    void drawDrawableAABB(const AABB &aabb,
                          const Color &color,
                          const RenderState &render_state) const;
    void drawLine(const vec3 &p1,
                  const vec3 &p2,
                  const Color &color,
                  const RenderState &render_state) const;
    void drawTriangle(const vec3 &v0,
                      const vec3 &v1,
                      const vec3 &v2,
                      const Color &color,
                      const RenderState &render_state) const;

    void queryTree(const BVH<entt::entity, entt::null> &tree,
                   entt::entity camera_entity,
                   std::vector<std::pair<float, entt::entity>> &entities,
                   std::vector<std::pair<float, entt::entity>> &transparent_entities);
    void updateTree(entt::entity entity);
    void removeTree(entt::entity entity);

private:
    BVH<entt::entity, entt::null> m_static_draw_tree;
    BVH<entt::entity, entt::null> m_dynamic_draw_tree;
    ComponentWatcher m_component_watcher;
    std::vector<std::pair<float, entt::entity>> m_visible_entities;
    std::vector<std::pair<float, entt::entity>> m_visible_transparent_entities;
    bool m_draw_dirty;
};

} // namespace ae

#endif // AE_DRAW_S_H
