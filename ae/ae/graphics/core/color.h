#ifndef AE_COLOR_H
#define AE_COLOR_H

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class Color
{
public:
    Color();
    Color(float red, float green, float blue, float alpha = 1.0f);
    Color(const vec4 &color);
    ~Color() = default;

    float getRed() const;
    void setRed(float red);

    float getGreen() const;
    void setGreen(float green);

    float getBlue() const;
    void setBlue(float blue);

    float getAlpha() const;
    void setAlpha(float alpha);

    const vec4 &getColor() const;
    void setColor(float red, float green, float blue, float alpha = 1.0f);
    void setColor(const vec4 &color);

    float getLuminance() const;

    Color darker(float factor = 0.7f) const;
    Color lighter(float factor = 1.3f) const;

    static Color fromInt(int32_t red, int32_t green, int32_t blue, int32_t alpha);

private:
    static void rgbToHsl(float r, float g, float b, float &h, float &s, float &l);
    static void hslToRgb(float h, float s, float l, float &r, float &g, float &b);
    static float hueToRgb(float p, float q, float t);

public:
    static Color transparent;
    static Color black;
    static Color white;
    static Color red;
    static Color green;
    static Color blue;

private:
    vec4 m_color;
};

} // namespace ae

#endif // AE_COLOR_H
