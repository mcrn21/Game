#include "time.h"

namespace ae {

Time::Time()
    : m_microseconds{0}
{}

Time::Time(int64_t microseconds)
    : m_microseconds{microseconds}
{}

Time::Time(const Time &other)
    : m_microseconds{other.m_microseconds}
{}

Time &Time::operator=(const Time &other)
{
    m_microseconds = other.m_microseconds;
    return *this;
}

float Time::asSeconds() const
{
    return static_cast<float>(static_cast<double>(m_microseconds) / 1000000.0);
}

int32_t Time::asMilliseconds() const
{
    return static_cast<int32_t>(m_microseconds / 1000);
}

int64_t Time::asMicroseconds() const
{
    return m_microseconds;
}

Time seconds(float amount)
{
    return Time(static_cast<int64_t>(amount * 1000000));
}

Time milliseconds(int32_t amount)
{
    return Time(static_cast<int64_t>(amount) * 1000);
}

Time microseconds(int64_t amount)
{
    return Time(amount);
}

bool operator==(Time left, Time right)
{
    return left.asMicroseconds() == right.asMicroseconds();
}

bool operator!=(Time left, Time right)
{
    return left.asMicroseconds() != right.asMicroseconds();
}

bool operator<(Time left, Time right)
{
    return left.asMicroseconds() < right.asMicroseconds();
}

bool operator>(Time left, Time right)
{
    return left.asMicroseconds() > right.asMicroseconds();
}

bool operator<=(Time left, Time right)
{
    return left.asMicroseconds() <= right.asMicroseconds();
}

bool operator>=(Time left, Time right)
{
    return left.asMicroseconds() >= right.asMicroseconds();
}

Time operator-(Time right)
{
    return microseconds(-right.asMicroseconds());
}

Time operator+(Time left, Time right)
{
    return microseconds(left.asMicroseconds() + right.asMicroseconds());
}

Time &operator+=(Time &left, Time right)
{
    return left = left + right;
}

Time operator-(Time left, Time right)
{
    return microseconds(left.asMicroseconds() - right.asMicroseconds());
}

Time &operator-=(Time &left, Time right)
{
    return left = left - right;
}

Time operator*(Time left, float right)
{
    return seconds(left.asSeconds() * right);
}

Time operator*(Time left, int64_t right)
{
    return microseconds(left.asMicroseconds() * right);
}

Time operator*(float left, Time right)
{
    return right * left;
}

Time operator*(int64_t left, Time right)
{
    return right * left;
}

Time &operator*=(Time &left, float right)
{
    return left = left * right;
}

Time &operator*=(Time &left, int64_t right)
{
    return left = left * right;
}

Time operator/(Time left, float right)
{
    return seconds(left.asSeconds() / right);
}

Time operator/(Time left, int64_t right)
{
    return microseconds(left.asMicroseconds() / right);
}

Time &operator/=(Time &left, float right)
{
    return left = left / right;
}

Time &operator/=(Time &left, int64_t right)
{
    return left = left / right;
}

float operator/(Time left, Time right)
{
    return left.asSeconds() / right.asSeconds();
}

Time operator%(Time left, Time right)
{
    return microseconds(left.asMicroseconds() % right.asMicroseconds());
}

Time &operator%=(Time &left, Time right)
{
    return left = left % right;
}

} // namespace ae
