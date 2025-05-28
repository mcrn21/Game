#include "animation.h"
#include "app.h"

#include <spdlog/spdlog.h>

namespace ae {

Animation::Animation(const Time &duration, const std::function<float(float)> &easing)
    : m_animation_manager{nullptr}
    , m_duration{duration}
    , m_finished{false}
    , m_paused{false}
    , m_stopped{false}
    , m_easing{easing}
{}

void Animation::start()
{
    App::getInstance().getAnimationManager()->add(sharedFromThis());
}

void Animation::pause()
{
    m_paused = true;
}

void Animation::resume()
{
    m_paused = false;
}

void Animation::stop()
{
    m_stopped = true;
}

bool Animation::isFinished() const
{
    return m_finished || m_stopped;
}

bool Animation::update(const Time &dt)
{
    if (m_finished || m_stopped)
        return true;
    if (m_paused)
        return false;

    // spdlog::debug("m_elapsed: {}", m_elapsed.asSeconds());

    m_elapsed += dt;
    float t = std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
    apply(m_easing(t));

    if (m_elapsed >= m_duration) {
        apply(1.0f);
        m_finished = true;
        return true;
    }

    return false;
}

FloatAnimation::FloatAnimation(float from,
                               float to,
                               Time duration,
                               const std::function<void(float)> &setter,
                               const std::function<float(float)> &easing)
    : Animation{duration, easing}
    , m_from{from}
    , m_to{to}
    , m_setter{setter}

{}

float FloatAnimation::getFrom() const
{
    return m_from;
}

void FloatAnimation::setFrom(float from)
{
    m_from = from;
}

float FloatAnimation::getTo() const
{
    return m_to;
}

void FloatAnimation::setTo(float to)
{
    m_to = to;
}

void FloatAnimation::apply(float t)
{
    float value = m_from + (m_to - m_from) * t;
    m_setter(value);
}

} // namespace ae
