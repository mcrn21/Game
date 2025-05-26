#include "system.h"
#include "scene.h"

namespace ae {

System::System(Scene *scene)
    : SceneContext{&scene->m_data}
    , m_scene{scene}
{}

Scene *System::getScene() const
{
    return m_scene;
}

entt::registry &System::getRegistry()
{
    return m_scene->getRegistry();
}

const entt::registry &System::getRegistry() const
{
    return m_scene->getRegistry();
}

} // namespace ae
