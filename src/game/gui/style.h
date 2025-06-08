#ifndef STYLE_H
#define STYLE_H

#include <ae/graphics/core/batch_2d.h>

using namespace ae;

struct Style
{
    struct Palette
    {
        static Color getOn(const Color &color); // Получить цвет текста и т.д. на фоне
        static Color getInverseOn(
            const Color &color); // Цвет текста в зависимости от фона, но без фона

        static Color getLighter(const Color &color); // Для текста и т.д.
        static Color getDarker(const Color &color);  // Для текста и т.д.

        static Color main_menu_bg;

        static Color primary;   // 1 цвет акцента
        static Color secondary; // 2 цвет акцента
        static Color tertiary;  // 3 цвет акцента
    };

    struct Metrics
    {
        static float main_fake_terminal_offset;
        static float main_fake_terminal_font_pixel_height;
    };

    // Draw funcs
    static void drawFakeTerminalButtonBg(const vec2 &pos,
                                         const vec2 &size,
                                         const Color &bg_color,
                                         const Color &border_color,
                                         float fill,
                                         float left_border_thickness,
                                         float bg_offset,
                                         Batch2D &batch_2d);

    static void drawTabButtonBg(const vec2 &pos,
                                const vec2 &size,
                                const Color &bg_color,
                                const Color &corner_color,
                                float corner_thickness,
                                float corner_width,
                                Batch2D &batch_2d);

    static void drawProgressBarBg(const vec2 &pos,
                                  const vec2 &size,
                                  const Color &bg_color,
                                  const Color &corner_color,
                                  float corner_thickness,
                                  float corner_width,
                                  float fill,
                                  Batch2D &batch_2d);
};

#endif // STYLE_H
