#include "label.h"
#include "gui.h"

namespace ae::gui {

Label::Label()
    : Control{}
{}

const String &Label::getString() const
{
    return m_string;
}

void Label::setString(const String &string)
{
    m_string = string;
    repaint();
}

void Label::onButtonPressed(ButtonCode button)
{
    clicked();
}

void Label::drawControl(Batch2D &batch_2d)
{
    if (!getGui())
        return;

    batch_2d.drawText(m_string, vec2{0.0f, 0.0f}, Color::white, getGui()->getDefaultFont());
}

} // namespace ae::gui
