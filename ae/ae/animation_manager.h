#ifndef AE_ANIMATION_MANAGER_H
#define AE_ANIMATION_MANAGER_H

#include "animation.h"
#include "system/time.h"

#include <memory>

namespace ae {

class AnimationManager
{
public:
    AnimationManager();
    ~AnimationManager() = default;

    void add(const std::shared_ptr<Animation> &animation);
    void update(const Time &dt);

private:
    std::vector<std::shared_ptr<Animation>> m_animations;
};

} // namespace ae

#endif // AE_ANIMATION_MANAGER_H
