#include "button_base.h"

namespace ae::gui {

ButtonBase::ButtonBase(EngineContext &engine_context)
    : Control{engine_context}
    , m_checkable{false}
    , m_checked{false}
    , m_line_spacing{0.0f}
{}

bool ButtonBase::isCheckable() const
{
    return m_checkable;
}

void ButtonBase::setCheckable(bool checkable)
{
    m_checkable = checkable;
}

bool ButtonBase::isChecked() const
{
    return m_checked;
}

void ButtonBase::setChecked(bool checked)
{
    if (m_checked != checked) {
        m_checked = checked;
        toggled(checked);
        repaint();
    }
}

const String &ButtonBase::getString() const
{
    return m_string;
}

void ButtonBase::setString(const String &string)
{
    m_string = string;
    updateImplicitSize();
    repaint();
}

float ButtonBase::getLineSpacing() const
{
    return m_line_spacing;
}

void ButtonBase::setLineSpacing(float line_spacing)
{
    m_line_spacing = line_spacing;
    updateImplicitSize();
    repaint();
}

void ButtonBase::onButtonReleased(ButtonCode button)
{
    if (button == ButtonCode::BUTTON_0) {
        clicked();
        if (m_checkable) {
            m_checked = !m_checked;
            toggled(m_checked);
            repaint();
        }
    }
}

void ButtonBase::updateImplicitSize()
{
    vec2 text_size
        = getFont()->getFontPage(getFontPixelHeight())->getTextSize(m_string, m_line_spacing);

    const auto &padding = getPadding();
    text_size.x += padding.x + padding.z;
    text_size.y += padding.y + padding.w;

    setImplicitSize(text_size);
}

} // namespace ae::gui
