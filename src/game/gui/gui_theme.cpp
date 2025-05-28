#include "gui_theme.h"

// Palette
// Main menu
Color GuiTheme::Palette::main_menu_bg = Color::fromInt(0, 0, 0, 180);

// Main menu button
Color GuiTheme::Palette::main_menu_button_border = Color::fromInt(243, 181, 98, 255);
Color GuiTheme::Palette::main_menu_button_hovered_bg = Color::fromInt(173, 111, 28, 255);
Color GuiTheme::Palette::main_menu_button_text = Color::white;

// Button
Color GuiTheme::Palette::button_bg = Color::fromInt(15, 8, 3, 220);
Color GuiTheme::Palette::button_hovered_bg = Color::fromInt(28, 69, 71, 220);
Color GuiTheme::Palette::button_border = Color::fromInt(243, 181, 98, 255);
Color GuiTheme::Palette::button_hovered_border = Color::fromInt(213, 53, 43, 255);
Color GuiTheme::Palette::button_text = Color::white;

// Dialog
Color GuiTheme::Palette::dialog_bg = Color::fromInt(15, 8, 3, 220);
Color GuiTheme::Palette::dialog_border = Color::fromInt(243, 181, 98, 255);
Color GuiTheme::Palette::dialog_border_2 = Color::fromInt(173, 111, 28, 255);
Color GuiTheme::Palette::dialog_border_3 = Color::fromInt(213, 53, 43, 255);

// Metrics
// Main menu button
float GuiTheme::Metrics::main_menu_button_font_pixel_size = 28.0f;
float GuiTheme::Metrics::main_menu_button_height = 36.0f;
float GuiTheme::Metrics::main_menu_button_border_thickness = 10.0f;
float GuiTheme::Metrics::main_menu_buttons_corner_offset = 64.0f;
float GuiTheme::Metrics::main_menu_buttons_spacing = 20.0f;

// Buttons
float GuiTheme::Metrics::button_font_pixel_size = 28.0f;
vec2 GuiTheme::Metrics::button_padding = vec2{28.0f, 22.0f};

// Dialog
float GuiTheme::Metrics::dialog_font_pixel_size = 28.0f;
float GuiTheme::Metrics::dialog_text_padding = 36.0f;
float GuiTheme::Metrics::dialog_button_height = 36.0f;
vec2 GuiTheme::Metrics::dialog_button_padding = vec2{28.0f, 6.0f};
float GuiTheme::Metrics::dialog_buttons_spacing = 48.0f;
float GuiTheme::Metrics::dialog_buttons_bottom_offset = 28.0f;

GuiTheme::Palette GuiTheme::palette;
GuiTheme::Metrics GuiTheme::metrics;

void GuiTheme::drawMainMenuButtonBg(const vec2 &pos,
                                    const vec2 &size,
                                    const Color &bg_color,
                                    const Color &border_color,
                                    float fill,
                                    Batch2D &batch_2d)
{
    batch_2d.drawRect(pos, vec2{metrics.main_menu_button_border_thickness, size.y}, border_color);

    batch_2d.drawRect(vec2{pos.x + metrics.main_menu_button_border_thickness * 1.4f, pos.y},
                      vec2{(size.x - metrics.main_menu_button_border_thickness * 1.4f) * fill,
                           size.y},
                      bg_color);
}

void GuiTheme::drawDialogBg(const vec2 &pos, const vec2 &size, Batch2D &batch_2d)
{
    float bg_offset = 2.0f;
    float min_size = std::min(size.x, size.y);
    float border_thickness = std::min(6.0f, min_size * 0.05f);
    vec4 corners{min_size * 0.12f, min_size * 0.06f, min_size * 0.06f, min_size * 0.06f};

    batch_2d.drawChamferedRect(pos + bg_offset, size - bg_offset * 2.0f, corners, palette.dialog_bg);

    std::vector<vec2> points;
    float border_offset = 8.0f;

    float b_space = min_size * 0.05f;

    points = {vec2{pos.x + size.x * 0.7f, pos.y + size.y},
              vec2{pos.x + corners.w, pos.y + size.y},
              vec2{pos.x, pos.y + size.y - corners.w},
              vec2{pos.x, pos.y + corners.x},
              vec2{pos.x + corners.x, pos.y},
              vec2{pos.x + size.x - corners.y, pos.y},
              vec2{pos.x + size.x, pos.y + corners.y},
              vec2{pos.x + size.x, pos.y + size.y * 0.7f}};
    batch_2d.drawPath(points, border_thickness, palette.dialog_border);

    points = {vec2{pos.x + size.x, pos.y + size.y * 0.7f + b_space},
              vec2{pos.x + size.x, pos.y + size.y - corners.z},
              vec2{pos.x + size.x - corners.z, pos.y + size.y},
              vec2{pos.x + size.x * 0.7f + b_space, pos.y + size.y}};
    batch_2d.drawPath(points, border_thickness, palette.dialog_border_3);

    float inline_border_offset = border_thickness * 2.0f;

    points = {vec2{pos.x + inline_border_offset, pos.y + size.y * 0.3f},
              vec2{pos.x + inline_border_offset, pos.y + corners.x + border_thickness},
              vec2{pos.x + corners.x + border_thickness, pos.y + inline_border_offset},
              vec2{pos.x + size.x * 0.3f, pos.y + inline_border_offset}};
    batch_2d.drawPath(points, border_thickness * 0.5f, palette.dialog_border_2);

    points = {vec2{pos.x + size.x - inline_border_offset, pos.y + size.y * 0.8f},
              vec2{pos.x + size.x - inline_border_offset,
                   pos.y + size.y - corners.z - border_thickness},
              vec2{pos.x + size.x - corners.z - border_thickness,
                   pos.y + size.y - inline_border_offset},
              vec2{pos.x + size.x * 0.8f, pos.y + size.y - inline_border_offset}};
    batch_2d.drawPath(points, border_thickness * 0.5f, palette.dialog_border_3);
}

void GuiTheme::drawButtonBg(const vec2 &pos,
                            const vec2 &size,
                            const Color &bg_color,
                            const Color &border_color,
                            Batch2D &batch_2d)
{
    float min_size = std::min(size.x, size.y);
    float bg_offset = 1.0f;
    float border_thickness = std::min(6.0f, min_size * 0.05f);
    float border_width = min_size * 0.2f;
    vec4 corners{min_size * 0.12f, min_size * 0.06f, min_size * 0.06f, min_size * 0.06f};

    batch_2d.drawRect(pos + bg_offset, size - bg_offset * 2.0f, bg_color);

    std::vector<vec2> points;
    points = {vec2{pos.x, pos.y + border_width},
              vec2{pos.x, pos.y},
              vec2{pos.x + border_width, pos.y}};
    batch_2d.drawPath(points, border_thickness, border_color);

    points = {vec2{pos.x + size.x - border_width, pos.y},
              vec2{pos.x + size.x, pos.y},
              vec2{pos.x + size.x, pos.y + border_width}};
    batch_2d.drawPath(points, border_thickness, border_color);

    points = {vec2{pos.x + size.x, pos.y + size.y - border_width},
              vec2{pos.x + size.x, pos.y + size.y},
              vec2{pos.x + size.x - border_width, pos.y + size.y}};
    batch_2d.drawPath(points, border_thickness, border_color);

    points = {vec2{pos.x + border_width, pos.y + size.y},
              vec2{pos.x, pos.y + size.y},
              vec2{pos.x, pos.y + size.y - border_width}};
    batch_2d.drawPath(points, border_thickness, border_color);
}
