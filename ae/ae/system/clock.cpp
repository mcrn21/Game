#include "clock.h"

#include <chrono>

namespace ae {

Clock::Clock()
    : m_start_time{getCurrentTime()}
{}

Time Clock::getElapsedTime() const
{
    return getCurrentTime() - m_start_time;
}

Time Clock::restart()
{
    Time current_time = getCurrentTime();
    Time elapsed = current_time - m_start_time;
    m_start_time = current_time;

    return elapsed;
}

Time Clock::getCurrentTime()
{
    int64_t microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::steady_clock::now().time_since_epoch())
                                           .count();
    return Time{static_cast<int64_t>(microseconds_since_epoch)};
}

} // namespace ae
