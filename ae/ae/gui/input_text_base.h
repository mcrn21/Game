#ifndef AE_GUI_INPUT_TEXT_BASE_H
#define AE_GUI_INPUT_TEXT_BASE_H

#include "../system/string.h"
#include "control.h"

namespace ae::gui {

class InputTextBase : public Control
{
public:
    InputTextBase();
    ~InputTextBase() = default;

    const String &getString() const;
    void setString(const String &string);

    void onKeyHeld(KeyCode code);
    void onKeyReleased(KeyCode code);
    void onCodepointInputed(uint32_t codepoint);

private:
    String m_string;
};

} // namespace ae::gui

#endif // AE_GUI_INPUT_TEXT_BASE_H
