#include "style.h"

// Palette
// Funcs
Color Style::Palette::getOn(const Color &color)
{
    return color.getLuminance() >= 0.5f ? getDarker(color) : getLighter(color);
}

Color Style::Palette::getInverseOn(const Color &color)
{
    return color.getLuminance() >= 0.5f ? getLighter(color) : getDarker(color);
}

Color Style::Palette::getLighter(const Color &color)
{
    return color.lighter(0.86f / (color.getLuminance() + 0.01) + 0.1);
}

Color Style::Palette::getDarker(const Color &color)
{
    return color.darker(1.05 - std::max(1.2f * color.getLuminance(), 1.0f));
}

// Colors
Color Style::Palette::main_menu_bg = Color::fromInt(0, 0, 0, 180);

Color Style::Palette::primary = Color::fromInt(219, 198, 111, 255);
Color Style::Palette::secondary = Color::fromInt(209, 198, 161, 255);
Color Style::Palette::tertiary = Color::fromInt(169, 208, 179, 255);

// Metrics
float Style::Metrics::main_fake_terminal_offset = 64.0f;
float Style::Metrics::main_fake_terminal_font_pixel_height = 36.0f;

// Draw funcs
void Style::drawFakeTerminalButtonBg(const vec2 &pos,
                                     const vec2 &size,
                                     const Color &bg_color,
                                     const Color &border_color,
                                     float fill,
                                     float left_border_thickness,
                                     float bg_offset,
                                     Batch2D &batch_2d)
{
    batch_2d.drawRect(pos, vec2{left_border_thickness, size.y}, border_color);
    batch_2d.drawRect(vec2{pos.x + left_border_thickness + bg_offset, pos.y},
                      vec2{(size.x - left_border_thickness + bg_offset) * fill, size.y},
                      bg_color);
}

void Style::drawTabButtonBg(const vec2 &pos,
                            const vec2 &size,
                            const Color &bg_color,
                            const Color &corner_color,
                            float corner_thickness,
                            float corner_width,
                            Batch2D &batch_2d)
{
    batch_2d.drawRect(pos, size, bg_color);

    float corner_offset = corner_thickness / 2.0f;

    std::vector<vec2> points;
    points = {vec2{pos.x + corner_offset, pos.y + corner_width + corner_offset},
              vec2{pos.x + corner_offset, pos.y + corner_offset},
              vec2{pos.x + corner_width + corner_offset, pos.y + corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + size.x - corner_width - corner_offset, pos.y + corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + corner_width + corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + size.x - corner_offset, pos.y + size.y - corner_width - corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + size.x - corner_width - corner_offset, pos.y + size.y - corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + corner_width + corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + corner_offset, pos.y + size.y - corner_width - corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);
}

void Style::drawProgressBarBg(const vec2 &pos,
                              const vec2 &size,
                              const Color &bg_color,
                              const Color &corner_color,
                              float corner_thickness,
                              float corner_width,
                              float fill,
                              Batch2D &batch_2d)
{
    float corner_offset = corner_thickness / 2.0f;

    std::vector<vec2> points;
    points = {vec2{pos.x + corner_offset, pos.y + corner_width + corner_offset},
              vec2{pos.x + corner_offset, pos.y + corner_offset},
              vec2{pos.x + corner_width + corner_offset, pos.y + corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + size.x - corner_width - corner_offset, pos.y + corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + corner_width + corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + size.x - corner_offset, pos.y + size.y - corner_width - corner_offset},
              vec2{pos.x + size.x - corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + size.x - corner_width - corner_offset, pos.y + size.y - corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    points = {vec2{pos.x + corner_width + corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + corner_offset, pos.y + size.y - corner_offset},
              vec2{pos.x + corner_offset, pos.y + size.y - corner_width - corner_offset}};
    batch_2d.drawPath(points, corner_thickness, corner_color);

    batch_2d.drawRect(pos, vec2{size.x * fill, size.y}, bg_color);
}
