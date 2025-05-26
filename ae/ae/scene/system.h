#ifndef AE_SYSTEM_H
#define AE_SYSTEM_H

#include "scene_context.h"

#include "entt/entt.hpp"

namespace ae {

class Scene;

class System : public SceneContext
{
public:
    System(Scene *scene);
    virtual ~System() = default;

    Scene *getScene() const;
    entt::registry &getRegistry();
    const entt::registry &getRegistry() const;

private:
    Scene *m_scene;
};

} // namespace ae

#endif // AE_SYSTEM_H
