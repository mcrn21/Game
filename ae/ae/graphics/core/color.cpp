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

float Color::getLuminance() const
{
    float lightness = (std::max({m_color.r, m_color.g, m_color.b})
                       + std::min({m_color.r, m_color.g, m_color.b}))
                      / 2.0f;
    return lightness;
}

Color Color::darker(float factor) const
{
    float h, s, l;
    rgbToHsl(m_color.r, m_color.g, m_color.b, h, s, l);
    l = std::clamp(l * factor, 0.0f, 1.0f);

    float r, g, b;
    hslToRgb(h, s, l, r, g, b);
    return Color{r, g, b, m_color.a};
}

Color Color::lighter(float factor) const
{
    float h, s, l;
    rgbToHsl(m_color.r, m_color.g, m_color.b, h, s, l);
    l = std::clamp(l * factor, 0.0f, 1.0f);

    float r, g, b;
    hslToRgb(h, s, l, r, g, b);
    return Color(r, g, b, m_color.a);
}

Color Color::fromInt(int32_t red, int32_t green, int32_t blue, int32_t alpha)
{
    return Color{static_cast<float>(red) / 255.0f,
                 static_cast<float>(green) / 255.0f,
                 static_cast<float>(blue) / 255.0f,
                 static_cast<float>(alpha) / 255.0f};
}

void Color::rgbToHsl(float r, float g, float b, float &h, float &s, float &l)
{
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    l = (max + min) / 2.0f;

    if (max == min) {
        h = s = 0.0f; // achromatic
    } else {
        float d = max - min;
        s = (l > 0.5f) ? (d / (2.0f - max - min)) : (d / (max + min));

        if (max == r)
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (max == g)
            h = (b - r) / d + 2.0f;
        else
            h = (r - g) / d + 4.0f;

        h /= 6.0f;
    }
}

void Color::hslToRgb(float h, float s, float l, float &r, float &g, float &b)
{
    if (s == 0.0f) {
        r = g = b = l; // achromatic
    } else {
        float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
        float p = 2.0f * l - q;
        r = hueToRgb(p, q, h + 1.0f / 3.0f);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1.0f / 3.0f);
    }
}

float Color::hueToRgb(float p, float q, float t)
{
    if (t < 0.0f)
        t += 1.0f;
    if (t > 1.0f)
        t -= 1.0f;
    if (t < 1.0f / 6.0f)
        return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f)
        return q;
    if (t < 2.0f / 3.0f)
        return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

} // namespace ae
