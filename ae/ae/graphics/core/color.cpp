#include "color.h"

#include <algorithm>

namespace ae {

Color Color::transparent{0.0f, 0.0f, 0.0f, 0.0f};
Color Color::black{0.0f, 0.0f, 0.0f, 1.0f};
Color Color::white(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::red{1.0f, 0.0f, 0.0f, 1.0f};
Color Color::green{0.0f, 1.0f, 0.0f, 1.0f};
Color Color::blue{0.0f, 0.0f, 1.0f, 1.0f};

Color::Color() {}

Color::Color(float red, float green, float blue, float alpha)
    : m_color{red, green, blue, alpha}
{}

Color::Color(const vec4 &color)
    : m_color{color}
{}

float Color::getRed() const
{
    return m_color.r;
}

void Color::setRed(float red)
{
    red = std::clamp(red, 0.0f, 1.0f);
    m_color.r = red;
}

float Color::getGreen() const
{
    return m_color.g;
}

void Color::setGreen(float green)
{
    green = std::clamp(green, 0.0f, 1.0f);
    m_color.g = green;
}

float Color::getBlue() const
{
    return m_color.b;
}

void Color::setBlue(float blue)
{
    blue = std::clamp(blue, 0.0f, 1.0f);
    m_color.b = blue;
}

float Color::getAlpha() const
{
    return m_color.a;
}

void Color::setAlpha(float alpha)
{
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    m_color.a = alpha;
}

const vec4 &Color::getColor() const
{
    return m_color;
}

void Color::setColor(float red, float green, float blue, float alpha)
{
    setRed(red);
    setGreen(green);
    setBlue(blue);
    setAlpha(alpha);
}

void Color::setColor(const vec4 &color)
{
    setColor(color.r, color.g, color.b, color.a);
}

Color Color::fromInt(int32_t red, int32_t green, int32_t blue, int32_t alpha)
{
    return Color{static_cast<float>(red) / 255.0f,
                 static_cast<float>(green) / 255.0f,
                 static_cast<float>(blue) / 255.0f,
                 static_cast<float>(alpha) / 255.0f};
}

} // namespace ae
