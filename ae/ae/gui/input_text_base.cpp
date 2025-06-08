#include "input_text_base.h"

namespace ae::gui {

InputTextBase::InputTextBase(EngineContext &engine_context)
    : Control{engine_context}
{}

const String &InputTextBase::getString() const
{
    return m_string;
}

void InputTextBase::setString(const String &string)
{
    m_string = string;
    repaint();
}

void InputTextBase::onKeyHeld(KeyCode code)
{
    if (code == KeyCode::BACKSPACE && !m_string.isEmpty()) {
        m_string.erase(m_string.getSize() - 1);
        repaint();
    }
}

void InputTextBase::onKeyReleased(KeyCode code)
{
    if (code == KeyCode::BACKSPACE && !m_string.isEmpty()) {
        m_string.erase(m_string.getSize() - 1);
        repaint();
    }
}

void InputTextBase::onCodepointInputed(uint32_t codepoint)
{
    m_string += codepoint;
    repaint();
}

} // namespace ae::gui
