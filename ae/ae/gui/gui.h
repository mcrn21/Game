#ifndef AE_GUI_H
#define AE_GUI_H

#include "../graphics/core/font.h"
#include "../graphics/core/render_texture.h"
#include "../system/time.h"
#include "../window/input.h"
#include "control.h"
#include "label.h"

#include <glm/glm.hpp>

using namespace glm;
using namespace ae::gui;

namespace ae {

class Gui
{
public:
    Gui();
    ~Gui() = default;

    // Render texture
    const RenderTexture &getRenderTexture() const;
    void setRenderTextureSize(const ivec2 &size);

    std::shared_ptr<Control> top() const;
    void push(const std::shared_ptr<Control> &control);
    void pop();
    void clear();

    void update(const Time &dt);
    void draw() const;

    // Input events
    void onButtonPressed(ButtonCode button);
    void onButtonReleased(ButtonCode button);
    void onCursorMoved(int32_t x, int32_t y, int32_t delta_x, int32_t delta_y);

    static const std::shared_ptr<Font> &getDefaultFont();

private:
    std::shared_ptr<Control> getHoveredContol(const vec2 &pos,
                                              vec2 *control_global_position = nullptr) const;

private:
    RenderTexture m_render_texture;
    mat4 m_proj_mat;

    std::vector<std::shared_ptr<Control>> m_controls_stack;

    std::shared_ptr<Control> m_hovered_control;
    std::shared_ptr<Control> m_focused_control;

    std::shared_ptr<Label> m_fps_label;
};

} // namespace ae

#endif // AE_GUI_H
