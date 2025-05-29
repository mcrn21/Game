#ifndef AE_SCENE_H
#define AE_SCENE_H

#include "../graphics/scene/model.h"
#include "../system/time.h"
#include "scene_context.h"
#include "scene_data.h"

#include <entt/entt.hpp>

namespace ae {

class Scene : public SceneContext
{
    friend class System;

public:
    Scene();
    ~Scene();

    entt::registry &getRegistry();

    void tickUpdate(const Time &elapsed_time);
    void draw() const;

    sigslot::signal<> tickUpdated;

    void createPlayer(const s_ptr<Model> &model,
                      const mat4 &player_transform = mat4{1.0f},
                      const mat4 &model_transform = mat4{1.0f});

private:
    void drawSkybox() const;
    void drawShadow() const;
    void drawScene() const;

    // On camera patched
    void onCameraUpdated(entt::registry &registry, entt::entity entity);
    // On trasform patched
    void onTransformUpdated(entt::registry &registry, entt::entity entity);
    // On local aabb patched
    void onLocalAABBUpdated(entt::registry &registry, entt::entity entity);
    // On movement patched
    void onMovementConstructed(entt::registry &registry, entt::entity entity);
    void onMovementUpdated(entt::registry &registry, entt::entity entity);

private:
    SceneData m_data;
};

} // namespace ae

#endif // AE_SCENE_H
