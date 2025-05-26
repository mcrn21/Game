#ifndef AE_TIME_H
#define AE_TIME_H

#include <stdint.h>

namespace ae {

class Time
{
public:
    Time();
    Time(int64_t microseconds);
    Time(const Time &other);
    ~Time() = default;

    Time &operator=(const Time &other);

    float asSeconds() const;
    int32_t asMilliseconds() const;
    int64_t asMicroseconds() const;

private:
    int64_t m_microseconds;
};

Time seconds(float amount);
Time milliseconds(int32_t amount);
Time microseconds(int64_t amount);

bool operator==(Time left, Time right);
bool operator!=(Time left, Time right);

bool operator<(Time left, Time right);
bool operator>(Time left, Time right);

bool operator<=(Time left, Time right);
bool operator>=(Time left, Time right);

Time operator-(Time right);
Time operator+(Time left, Time right);
Time &operator+=(Time &left, Time right);
Time operator-(Time left, Time right);
Time &operator-=(Time &left, Time right);
Time operator*(Time left, float right);
Time operator*(Time left, int64_t right);
Time operator*(float left, Time right);
Time operator*(int64_t left, Time right);
Time &operator*=(Time &left, float right);
Time &operator*=(Time &left, int64_t right);
Time operator/(Time left, float right);
Time operator/(Time left, int64_t right);
Time &operator/=(Time &left, float right);
Time &operator/=(Time &left, int64_t right);
float operator/(Time left, Time right);
Time operator%(Time left, Time right);
Time &operator%=(Time &left, Time right);

} // namespace ae

#endif // AE_TIME_H
