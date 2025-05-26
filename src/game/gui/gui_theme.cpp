#include "gui_theme.h"

// Main menu
Color GuiTheme::main_menu_bg_color = Color::fromInt(0, 0, 0, 180);
vec2 GuiTheme::main_menu_button_size{256.0f, 32.0f};

// Main menu button
//Color::fromInt(255, 98, 93, 255);
Color GuiTheme::main_menu_button_color = Color::fromInt(253, 183, 68, 255);

void GuiTheme::drawMainMenuButtonBg(const vec2 &pos, const vec2 &size, float fill, Batch2D &batch_2d)
{
    batch_2d.drawRect(pos, vec2{main_menu_button_left_border, size.y}, main_menu_button_color);

    float rect_2_offset = main_menu_button_left_border + main_menu_button_left_border_2;
    batch_2d.drawRect(vec2{pos.x + rect_2_offset, pos.y},
                      vec2{(size.x - rect_2_offset) * fill, size.y},
                      main_menu_button_color);
}

// Frame
Color GuiTheme::frame_bg_color = Color::fromInt(15, 8, 3, 220);
Color GuiTheme::frame_border_color = Color::fromInt(115, 103, 77, 255);
Color GuiTheme::frame_border_color_2 = Color::fromInt(215, 182, 77, 255);

void GuiTheme::drawFrameBg(const vec2 &pos, const vec2 &size, Batch2D &batch_2d)
{
    float border_thickness = 6.0f;
    float border_thickness_2 = 6.0f;
    float border_width = size.x / 10.0f;
    float corner_border_width = size.x / 10.0f;

    float bg_offset = 3.0f;

    batch_2d.drawRectWithBorder(pos + bg_offset,
                                size - bg_offset * 2.0f,
                                frame_bg_color,
                                frame_border_color,
                                border_thickness);

    std::vector<vec2> points;

    float left_offset = 0.0f;
    float top_offset = 0.0f;

    // Left top corner
    points = {vec2{left_offset, 42.0f},
              vec2{left_offset, 8.0f},
              vec2{8.0f, top_offset},
              vec2{64.0f, top_offset}};
    batch_2d.drawSmoothPolyline(points, border_thickness_2, frame_border_color_2);

    // batch_2d.drawLine(vec2{6.0f, 0.0f}, vec2{40.0f, 0.0f}, border_thickness_2, frame_border_color_2);
    // batch_2d.drawLine(vec2{0.0f, 6.0f}, vec2{0.0f, 40.0f}, border_thickness_2, frame_border_color_2);

    // float h_border_left_offset = pos.x + border_thickness_2;
    // float h_border_top_offset = pos.y + border_thickness_2 * 1.5f;
    // float h_border_right_offset = pos.x + size.x - border_thickness_2;
    // float h_border_bottom_offset = pos.y + size.y - border_thickness_2 * 1.5;
    // float h_border_middle_offset = pos.x + size.x / 2.0f - border_width / 2.0f;

    // // Top
    // batch_2d.drawLine(vec2{h_border_left_offset, h_border_top_offset},
    //                   vec2{h_border_left_offset + corner_border_width * 3.0f, h_border_top_offset},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // float off = h_border_left_offset + corner_border_width * 3.0f + corner_border_width * 0.3f;
    // for (int32_t i = 0; i < 5; ++i) {
    //     float w = border_width * (0.1f * (5 - i));
    //     batch_2d.drawLine(vec2{off, h_border_top_offset},
    //                       vec2{off + w, h_border_top_offset},
    //                       border_thickness_2,
    //                       frame_border_color_2);
    //     off += w + corner_border_width * 0.3f;
    // }

    // // batch_2d.drawLine(vec2{h_border_middle_offset, h_border_top_offset},
    // //                   vec2{h_border_middle_offset + border_width * 3.0f, h_border_top_offset},
    // //                   border_thickness_2,
    // //                   frame_border_color_2);

    // batch_2d.drawLine(vec2{h_border_right_offset, h_border_top_offset},
    //                   vec2{h_border_right_offset - corner_border_width, h_border_top_offset},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // // Bottom
    // batch_2d.drawLine(vec2{h_border_left_offset, h_border_bottom_offset},
    //                   vec2{h_border_left_offset + corner_border_width, h_border_bottom_offset},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // // batch_2d.drawLine(vec2{h_border_middle_offset + border_width, h_border_bottom_offset},
    // //                   vec2{h_border_middle_offset - border_width * 2.0f, h_border_bottom_offset},
    // //                   border_thickness_2,
    // //                   frame_border_color_2);

    // batch_2d.drawLine(vec2{h_border_right_offset, h_border_bottom_offset},
    //                   vec2{h_border_right_offset - corner_border_width * 3.0f,
    //                        h_border_bottom_offset},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // float v_border_left_offset = pos.x + border_thickness_2 * 1.5f;
    // float v_border_top_offset = pos.y + border_thickness_2 * 2.0f;
    // float v_border_right_offset = pos.x + size.x - border_thickness_2 * 1.5f;
    // float v_border_bottom_offset = pos.y + size.y - border_thickness_2 * 2.0f;
    // float v_border_middle_offset = pos.y + size.y / 2.0f - border_width / 2.0f;

    // // Left
    // batch_2d.drawLine(vec2{v_border_left_offset, v_border_top_offset},
    //                   vec2{v_border_left_offset,
    //                        v_border_top_offset + corner_border_width - border_thickness_2},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // batch_2d.drawLine(vec2{v_border_left_offset, v_border_bottom_offset},
    //                   vec2{v_border_left_offset,
    //                        v_border_bottom_offset - corner_border_width * 2.0f},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // // Right
    // batch_2d.drawLine(vec2{v_border_right_offset, v_border_top_offset},
    //                   vec2{v_border_right_offset, v_border_top_offset + corner_border_width * 2.0f},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // batch_2d.drawLine(vec2{v_border_right_offset, v_border_bottom_offset},
    //                   vec2{v_border_right_offset,
    //                        v_border_bottom_offset - corner_border_width + border_thickness_2},
    //                   border_thickness_2,
    //                   frame_border_color_2);

    // std::vector<vec2> points = {vec2{50.0f},
    //                             vec2{70.0f, 20.0f},
    //                             vec2{100.0f, 80.0f},
    //                             vec2{150.0f, 30.0f}};
    // batch_2d.drawSmoothPolyline(points, 10.0f, Color::green);
}

// Exit dialog
vec2 GuiTheme::exit_dialog_size{500.0f, 300.0f};
