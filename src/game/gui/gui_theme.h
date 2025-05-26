#ifndef GUI_THEME_H
#define GUI_THEME_H

#include <ae/graphics/core/batch_2d.h>

#include <glm/glm.hpp>

using namespace glm;
using namespace ae;

struct GuiTheme
{
    // Main menu
    static Color main_menu_bg_color;
    static vec2 main_menu_button_size;
    static constexpr float main_menu_button_spacing = 24.0f;
    static constexpr float main_menu_button_offset_x = 64.0f;

    // Main menu button
    static constexpr float main_menu_button_left_border = 8.0f;
    static constexpr float main_menu_button_left_border_2 = 3.0f;
    static constexpr float main_menu_button_text_left_offset = 16.0f;

    static Color main_menu_button_color;

    static void drawMainMenuButtonBg(const vec2 &pos,
                                     const vec2 &size,
                                     float fill,
                                     Batch2D &batch_2d);

    // Frame
    static Color frame_bg_color;
    static Color frame_border_color;
    static Color frame_border_color_2;

    static void drawFrameBg(const vec2 &pos, const vec2 &size, Batch2D &batch_2d);

    // Exit dialog
    static vec2 exit_dialog_size;
};

#endif // GUI_THEME_H
