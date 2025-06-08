#include "label.h"

namespace ae::gui {

Label::Label(EngineContext &engine_context)
    : Control{engine_context}
    , m_line_spacing{0.0f}
    , m_color{Color::white}
{}

const String &Label::getString() const
{
    return m_string;
}

void Label::setString(const String &string)
{
    m_string = string;
    updateImplicitSize();
    repaint();
}

void Label::appendString(const String &string)
{
    m_string += string;
    updateImplicitSize();
    repaint();
}

float Label::getLineSpacing() const
{
    return m_line_spacing;
}

void Label::setLineSpacing(float line_spacing)
{
    m_line_spacing = line_spacing;
    updateImplicitSize();
    repaint();
}

const Color &Label::getColor() const
{
    return m_color;
}

void Label::setColor(const Color &color)
{
    m_color = color;
    repaint();
}

void Label::drawControl(Batch2D &batch_2d)
{
    batch_2d.drawText(m_string,
                      vec2{0.0f, 0.0f},
                      m_color,
                      getFont(),
                      getFontPixelHeight(),
                      m_line_spacing);
}

void Label::updateImplicitSize()
{
    vec2 text_size
        = getFont()->getFontPage(getFontPixelHeight())->getTextSize(m_string, m_line_spacing);

    const auto &padding = getPadding();
    text_size.x += padding.x + padding.z;
    text_size.y += padding.y + padding.w;

    setImplicitSize(text_size);
}

} // namespace ae::gui
