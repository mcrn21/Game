#include "button.h"
#include "gui_theme.h"

#include <ae/gui/gui.h>

Button::Button()
{
    m_font = Gui::getDefaultFont();
}

const std::shared_ptr<Font> &Button::getFont() const
{
    return m_font;
}

void Button::setFont(const std::shared_ptr<Font> &font)
{
    m_font = font;
}

const String &Button::getString() const
{
    return m_string;
}

void Button::setString(const String &string)
{
    m_string = string;
}

void Button::onStateChanged(int32_t)
{
    repaint();
}

void Button::onSizeChanged(const vec2 &)
{
    repaint();
}

void Button::onCursorEnter()
{
    repaint();
}

void Button::onCursorLeave()
{
    repaint();
}

void Button::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawFrameBg(vec2{0.0f}, getSize(), batch_2d);

    batch_2d.drawText(m_string,
                      vec2{0.0f, (getSize().y - m_font->getPixelHeight()) / 2.0f},
                      Color::white,
                      m_font);
}
