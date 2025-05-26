#ifndef AE_CLOCK_H
#define AE_CLOCK_H

#include "time.h"

namespace ae {

class Clock
{
public:
    Clock();
    ~Clock() = default;

    Time getElapsedTime() const;
    Time restart();

    static Time getCurrentTime();

private:
    Time m_start_time;
};

} // namespace ae

#endif // AE_CLOCK_H
