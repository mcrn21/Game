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

SharedPtr<Control> Gui::top() const
{
    if (m_controls_stack.empty())
        return nullptr;
    return m_controls_stack.back();
}

void Gui::push(const SharedPtr<Control> &control)
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
    auto hovered_control = m_hovered_control.lock();
    auto focused_control = m_focused_control.lock();

    if (!hovered_control) {
        if (focused_control)
            focused_control->setState(focused_control->getState() & ~Control::FOCUSED);
        m_focused_control.reset();
        return;
    }

    if (focused_control)
        focused_control->setState(focused_control->getState() & ~Control::FOCUSED);

    m_focused_control = m_hovered_control;

    focused_control = m_focused_control.lock();
    focused_control->onButtonPressed(button);
    focused_control->setState(focused_control->getState() | Control::FOCUSED | Control::PRESSED);
}

void Gui::onButtonReleased(ButtonCode button)
{
    auto focused_control = m_focused_control.lock();
    if (focused_control) {
        focused_control->onButtonReleased(button);
        focused_control->setState(focused_control->getState() & ~Control::PRESSED);
    }
}

void Gui::onCursorMoved(int32_t x, int32_t y, int32_t delta_x, int32_t delta_y)
{
    vec2 control_global_position{0.0f};
    auto c = getHoveredContol(vec2{x, y}, &control_global_position);

    auto hovered_control = m_hovered_control.lock();

    if (hovered_control && hovered_control != c) {
        hovered_control->onCursorLeave();
        hovered_control->setState(hovered_control->getState() & ~Control::HOVERED);
    }

    if (c && hovered_control != c) {
        c->onCursorEnter();
        c->setState(c->getState() | Control::HOVERED);
    }

    m_hovered_control = c;

    hovered_control = m_hovered_control.lock();
    if (hovered_control)
        hovered_control->onCursorMoved(vec2{x, y} - control_global_position);
}

void Gui::onKeyPressed(KeyCode code)
{
    auto focused_control = m_focused_control.lock();
    if (focused_control)
        focused_control->onKeyPressed(code);
}

void Gui::onKeyHeld(KeyCode code)
{
    auto focused_control = m_focused_control.lock();
    if (focused_control)
        focused_control->onKeyHeld(code);
}

void Gui::onKeyReleased(KeyCode code)
{
    auto focused_control = m_focused_control.lock();
    if (focused_control)
        focused_control->onKeyReleased(code);
}

void Gui::onCodepointInputed(uint32_t codepoint)
{
    auto focused_control = m_focused_control.lock();
    if (focused_control)
        focused_control->onCodepointInputed(codepoint);
}

const SharedPtr<Font> &Gui::getDefaultFont()
{
    static SharedPtr<Font> default_font;

    if (!default_font) {
        default_font = SharedPtr<Font>::create();
        default_font->loadFromMemory(reinterpret_cast<const uint8_t *>(
                                         b::embed<"fonts/default.ttf">().data()),
                                     b::embed<"fonts/default.ttf">().size());
    }

    return default_font;
}

SharedPtr<Control> Gui::getHoveredContol(const vec2 &pos, vec2 *control_global_position) const
{
    if (!m_controls_stack.empty() && m_controls_stack.back()->contains(pos)) {
        auto c = m_controls_stack.back()->findContolAtPos(pos, control_global_position);
        if (c && (!c->isEnable() || !c->isEnable()))
            return nullptr;
        return c;
    }
    return nullptr;
}

} // namespace ae
