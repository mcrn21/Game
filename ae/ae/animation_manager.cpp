#include "animation_manager.h"

namespace ae {

AnimationManager::AnimationManager() {}

void AnimationManager::add(const SharedPtr<Animation> &animation)
{
    if (!animation || animation->m_animation_manager != nullptr)
        return;

    animation->m_animation_manager = this;
    animation->m_elapsed = Time{};
    animation->m_paused = false;
    animation->m_stopped = false;
    animation->m_finished = false;

    m_animations.push_back(animation);
}

void AnimationManager::update(const Time &dt)
{
    m_animations.erase(std::remove_if(m_animations.begin(),
                                      m_animations.end(),
                                      [&](SharedPtr<Animation> &a) {
                                          bool result = a->update(dt);
                                          if (result)
                                              a->m_animation_manager = nullptr;
                                          return result;
                                      }),
                       m_animations.end());
}

} // namespace ae
