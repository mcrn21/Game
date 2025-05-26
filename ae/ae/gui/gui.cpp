#include "gui.h"
#include "../app.h"
#include "../graphics/core/default_shaders.h"
#include "../graphics/core/shader.h"

#include "battery/embed.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace ae {

Gui::Gui()
{
    m_render_texture.setClearColor(Color::transparent);
    m_fps_label = gui::Control::create<Label>();
    m_fps_label->m_gui = this;
    m_fps_label->setString(String("FPS: "));
}

const RenderTexture &Gui::getRenderTexture() const
{
    return m_render_texture;
}

void Gui::setRenderTextureSize(const ivec2 &size)
{
    m_render_texture.create(size, true);
    m_proj_mat = glm::ortho(0.0f,
                            static_cast<float>(size.x),
                            static_cast<float>(size.y),
                            0.0f,
                            -1.0f,
                            1.0f);

    for (auto &control : m_controls_stack)
        control->setSize(vec2{size.x, size.y});
}

std::shared_ptr<Control> Gui::top() const
{
    if (m_controls_stack.empty())
        return nullptr;
    return m_controls_stack.back();
}

void Gui::push(const std::shared_ptr<Control> &control)
{
    m_controls_stack.push_back(control);
    control->setSize(m_render_texture.getSize());
    control->updateGui(this);
}

void Gui::pop()
{
    if (!m_controls_stack.empty()) {
        m_controls_stack.back()->updateGui(nullptr);
        m_controls_stack.pop_back();
    }
}

void Gui::clear()
{
    m_controls_stack.clear();
}

void Gui::update(const Time &dt) {}

void Gui::draw() const
{
    m_render_texture.clear();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    Shader::use(*DefaultShaders::getGui());
    DefaultShaders::getGui()->uniformMatrix("u_projMat", m_proj_mat);

    for (const auto &control : m_controls_stack)
        control->draw(DefaultShaders::getGui().get(), mat4{1.0f});

    // Fps label
    m_fps_label->setString(String("FPS: " + std::to_string(App::getInstance().getFps())));
    m_fps_label->draw(DefaultShaders::getGui().get(), mat4{1.0f});

    Shader::unuse();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    m_render_texture.display();
}

void Gui::onButtonPressed(ButtonCode button)
{
    if (!m_hovered_control) {
        if (m_focused_control)
            m_focused_control->setState(m_focused_control->getState() & ~Control::FOCUSED);
        m_focused_control = nullptr;
        return;
    }

    if (m_focused_control)
        m_focused_control->setState(m_focused_control->getState() & ~Control::FOCUSED);

    m_focused_control = m_hovered_control;

    m_focused_control->onButtonPressed(button);
    m_focused_control->setState(m_focused_control->getState() | Control::FOCUSED | Control::PRESSED);
}

void Gui::onButtonReleased(ButtonCode button)
{
    if (m_focused_control) {
        m_focused_control->onButtonReleased(button);
        m_focused_control->setState(m_focused_control->getState() & ~Control::PRESSED);
    }
}

void Gui::onCursorMoved(int32_t x, int32_t y, int32_t delta_x, int32_t delta_y)
{
    vec2 control_global_position{0.0f};
    auto c = getHoveredContol(vec2{x, y}, &control_global_position);

    if (m_hovered_control && m_hovered_control != c) {
        m_hovered_control->onCursorLeave();
        m_hovered_control->setState(m_hovered_control->getState() & ~Control::HOVERED);
    }

    if (c && m_hovered_control != c) {
        c->onCursorEnter();
        c->setState(c->getState() | Control::HOVERED);
    }

    m_hovered_control = c;

    if (m_hovered_control)
        m_hovered_control->onCursorMoved(vec2{x, y} - control_global_position);
}

const std::shared_ptr<Font> &Gui::getDefaultFont()
{
    static std::shared_ptr<Font> default_font;

    if (!default_font) {
        default_font = std::make_shared<Font>();
        default_font->loadFromMemory(reinterpret_cast<const uint8_t *>(
                                         b::embed<"fonts/default.ttf">().data()),
                                     32.0f);
    }

    return default_font;
}

std::shared_ptr<Control> Gui::getHoveredContol(const vec2 &pos, vec2 *control_global_position) const
{
    if (!m_controls_stack.empty() && m_controls_stack.back()->isVisible()
        && m_controls_stack.back()->contains(pos))
        return m_controls_stack.back()->findContolAtPos(pos, control_global_position);
    return nullptr;
}

} // namespace ae
