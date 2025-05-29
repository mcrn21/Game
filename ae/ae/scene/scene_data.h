#ifndef AE_SCENE_DATA_H
#define AE_SCENE_DATA_H

#include "../graphics/core/render_texture.h"
#include "../system/memory.h"
#include "bvh.h"

#include <entt/entt.hpp>

namespace ae {

class RenderTarget;
class Collisions_S;
class Draw_S;
class Lights_S;
class Movement_S;
class Player_S;

struct SceneData
{
    RenderTexture render_texture;

    entt::registry registry;

    entt::entity active_camera = entt::null;
    entt::entity active_direct_light = entt::null;
    entt::entity active_skybox = entt::null;

    // Collisions tree
    BVH<entt::entity, entt::null> m_static_colliders_tree;
    BVH<entt::entity, entt::null> m_dynamic_colliders_tree;

    bool enable_shadow = false;

    u_ptr<Draw_S> draw_s;
    u_ptr<Collisions_S> collisions_s;
    u_ptr<Lights_S> lights_s;
    u_ptr<Player_S> player_s;
    u_ptr<Movement_S> movement_s;

    bool camera_dirty = true;
    bool scene_dirty = true;
};

} // namespace ae

#endif // AE_SCENE_DATA_H
