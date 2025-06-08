#include "control.h"
#include "gui.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <ranges>

namespace ae::gui {

Control::Control(EngineContext &engine_context)
    : EngineContextObject{engine_context}
    , m_gui{nullptr}
    , m_state{DEFAULT}
    , m_enable{true}
    , m_visible{true}
    , m_padding{0.0f}
    , m_implicit_size{32.0f}
    , m_size{32.0f}
    , m_position{0.0f}
    , m_transform{1.0f}
    , m_transform_dirty{true}
    , m_font_pixel_height{28.0f}
    , m_draw_dirty{true}
    , m_destroyed{false}
{
    m_font = Gui::getDefaultFont();
}

Control::~Control() {}

Gui *Control::getGui() const
{
    return m_gui;
}

s_ptr<Control> Control::getParent() const
{
    return m_parent.lock();
}

void Control::setParent(const s_ptr<Control> &parent)
{
    auto p = m_parent.lock();
    if (p) {
        p->m_children.erase(std::remove(p->m_children.begin(),
                                        p->m_children.end(),
                                        sharedFromThis()),
                            p->m_children.end());
    }

    m_parent = parent;
    p = m_parent.lock();
    if (p) {
        auto ptr = sharedFromThis();
        p->m_children.push_back(ptr);
        updateGui(p->m_gui);
    } else
        updateGui(nullptr);
}

const std::vector<s_ptr<Control> > &Control::getChildren() const
{
    return m_children;
}

int32_t Control::getState() const
{
    return m_state;
}

void Control::setState(int32_t state)
{
    m_state = state;
    repaint();
    onStateChanged(state);
}

bool Control::isEnable() const
{
    return m_enable;
}

void Control::setEnable(bool enable)
{
    m_enable = enable;
}

bool Control::isVisible() const
{
    return m_visible;
}

void Control::setVisible(bool visible)
{
    m_visible = true;
}

const vec4 &Control::getPadding() const
{
    return m_padding;
}

void Control::setPadding(const vec4 &padding)
{
    m_padding = padding;
}

const vec2 &Control::getImplicitSize() const
{
    return m_implicit_size;
}

void Control::setImplicitSize(const vec2 &implicit_size)
{
    m_implicit_size = implicit_size;
}

const vec2 &Control::getSize() const
{
    return m_size;
}

void Control::setSize(const vec2 &size)
{
    m_size = size;
    repaint();
    onSizeChanged(size);
}

const vec2 &Control::getPosition() const
{
    return m_position;
}

void Control::setPosition(const vec2 &position)
{
    m_position = position;
    m_transform_dirty = true;
}

const mat4 &Control::getTransform() const
{
    if (m_transform_dirty) {
        m_transform_dirty = false;
        m_transform = glm::translate(mat4{1.0f}, vec3{m_position.x, m_position.y, 0.0f});
    }

    return m_transform;
}

s_ptr<Control> Control::findContolAtPos(const vec2 &pos, vec2 *control_global_position)
{
    if (control_global_position)
        *control_global_position += m_position;

    for (const auto &child : std::views::reverse(m_children)) {
        if (child->contains(pos)) {
            auto c = child->findContolAtPos(pos - child->getPosition(), control_global_position);
            if (c)
                return c;
        }
    }
    return sharedFromThis();
}

bool Control::contains(const vec2 &pos) const
{
    return pos.x > m_position.x && pos.y > m_position.y && pos.x < (m_position.x + m_size.x)
           && pos.y < (m_position.y + m_size.y);
}

float Control::getFontPixelHeight() const
{
    return m_font_pixel_height;
}

void Control::setFontPixelHeight(float pixel_size)
{
    m_font_pixel_height = pixel_size;
    repaint();
    updateImplicitSize();
}

const s_ptr<Font> &Control::getFont() const
{
    return m_font;
}

void Control::setFont(const s_ptr<Font> &font)
{
    m_font = font;
    repaint();
    updateImplicitSize();
}

void Control::repaint()
{
    m_draw_dirty = true;
}

void Control::draw(Shader *shader, const mat4 &transform) const
{
    if (!m_visible)
        return;

    mat4 t = transform * getTransform();

    if (m_draw_dirty) {
        m_draw_dirty = false;
        m_batch_2d.begin();
        const_cast<Control *>(this)->drawControl(m_batch_2d);
        m_batch_2d.end();
    }

    shader->uniformMatrix("u_model", t);
    m_batch_2d.draw();

    for (const auto &child : m_children)
        child->draw(shader, t);
}

void Control::onCreated() {}

void Control::onStateChanged(int32_t state) {}

void Control::onSizeChanged(const vec2 &size) {}

void Control::onCursorEnter() {}

void Control::onCursorLeave() {}

void Control::onCursorMoved(const vec2 &position) {}

void Control::onButtonPressed(ButtonCode button) {}

void Control::onButtonReleased(ButtonCode button) {}

void Control::onKeyPressed(KeyCode code) {}

void Control::onKeyHeld(KeyCode code) {}

void Control::onKeyReleased(KeyCode code) {}

void Control::onCodepointInputed(uint32_t codepoint) {}

void Control::drawControl(Batch2D &batch_2d) {}

void Control::updateImplicitSize() {}

void Control::updateGui(Gui *gui)
{
    m_gui = gui;
    for (auto &child : m_children)
        child->updateGui(gui);
}

} // namespace ae::gui
