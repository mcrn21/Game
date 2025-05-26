#ifndef AE_ANIMATION_H
#define AE_ANIMATION_H

#include "system/time.h"

#include <functional>
#include <math.h>
#include <memory>

namespace ae {

namespace Easing {
inline float linear(float t)
{
    return t;
}

inline float easeInQuad(float t)
{
    return t * t;
}

inline float easeOutQuad(float t)
{
    return t * (2.0f - t);
}

inline float easeInOutQuad(float t)
{
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

inline float easeInCubic(float t)
{
    return t * t * t;
}

inline float easeOutCubic(float t)
{
    float p = t - 1.0f;
    return p * p * p + 1.0f;
}

inline float easeInOutCubic(float t)
{
    return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
}

inline float easeInSine(float t)
{
    return 1.0f - std::cos((t * M_PI) / 2.0f);
}

inline float easeOutSine(float t)
{
    return std::sin((t * M_PI) / 2.0f);
}

inline float easeInOutSine(float t)
{
    return -(std::cos(M_PI * t) - 1.0f) / 2.0f;
}

inline float easeInExpo(float t)
{
    return (t == 0.0f) ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
}

inline float easeOutExpo(float t)
{
    return (t == 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

inline float easeInOutExpo(float t)
{
    if (t == 0.0f)
        return 0.0f;
    if (t == 1.0f)
        return 1.0f;
    return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
                    : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

} // namespace Easing

class AnimationManager;

class Animation : public std::enable_shared_from_this<Animation>
{
    friend class AnimationManager;

public:
    Animation(const Time &duration, const std::function<float(float)> &easing = Easing::linear);
    virtual ~Animation() = default;

    void start();
    void pause();
    void resume();
    void stop();

    bool isFinished() const;

    bool update(const Time &dt);

    virtual void apply(float t) = 0;

protected:
    AnimationManager *m_animation_manager;
    Time m_duration;
    Time m_elapsed;
    bool m_finished;
    bool m_paused;
    bool m_stopped;
    std::function<float(float)> m_easing;
};

class FloatAnimation : public Animation
{
public:
    FloatAnimation(float from,
                   float to,
                   Time duration,
                   const std::function<void(float)> &setter,
                   const std::function<float(float)> &easing = Easing::linear);
    ~FloatAnimation() = default;

    float getFrom() const;
    void setFrom(float from);

    float getTo() const;
    void setTo(float to);

    void apply(float t);

private:
    float m_from;
    float m_to;
    std::function<void(float)> m_setter;
};

} // namespace ae

#endif // AE_ANIMATION_H
