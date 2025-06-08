#ifndef GUI_THEME_H
#define GUI_THEME_H

#include <ae/graphics/core/batch_2d.h>

#include <glm/glm.hpp>

using namespace glm;
using namespace ae;


struct GuiTheme
{
    static struct Palette
    {
        // Main menu
        static Color main_menu_bg;

        // Main menu button
        static Color main_menu_button_border;
        static Color main_menu_button_hovered_bg;
        static Color main_menu_button_text;

        // Button
        static Color button_bg;
        static Color button_hovered_bg;
        static Color button_border;
        static Color button_hovered_border;
        static Color button_text;

        // Dialog
        static Color dialog_bg;
        static Color dialog_border;
        static Color dialog_border_2;
        static Color dialog_border_3;

        // Progress bar
        static Color progress_bar_bg;
        static Color progress_bar_border;

    } palette;

    static struct Metrics
    {
        // Load level
        static vec2 load_level_progress_bar_size;
        static float load_level_progress_bar_bottom_offset;

        // Button
        static float button_font_pixel_size;
        static vec2 button_padding;

        // Dialog
        static float dialog_font_pixel_size;
        static float dialog_text_padding;
        static float dialog_button_height;
        static vec2 dialog_button_padding;
        static float dialog_buttons_spacing;
        static float dialog_buttons_bottom_offset;
    } metrics;

    /////

    static void drawMainMenuButtonBg(const vec2 &pos,
                                     const vec2 &size,
                                     const Color &bg_color,
                                     const Color &border_color,
                                     float fill,
                                     Batch2D &batch_2d);

    static void drawDialogBg(const vec2 &pos, const vec2 &size, Batch2D &batch_2d);

    static void drawButtonBg(const vec2 &pos,
                             const vec2 &size,
                             const Color &bg_color,
                             const Color &border_color,
                             Batch2D &batch_2d);

    static void drawProgressBarBg(const vec2 &pos,
                                  const vec2 &size,
                                  const Color &bg_color,
                                  const Color &border_color,
                                  float fill,
                                  Batch2D &batch_2d);
};

#endif // GUI_THEME_H
