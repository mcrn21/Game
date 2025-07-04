#ifndef AE_GUI_BUTTON_H
#define AE_GUI_BUTTON_H

#include "../system/signal.h"
#include "../system/string.h"
#include "control.h"

namespace ae::gui {

class ButtonBase : public Control
{
public:
    ButtonBase(EngineContext &engine_context);
    ~ButtonBase() = default;

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isChecked() const;
    void setChecked(bool checked);

    const String &getString() const;
    void setString(const String &string);

    float getLineSpacing() const;
    void setLineSpacing(float line_spacing);

    void onButtonReleased(ButtonCode button);

protected:
    void updateImplicitSize();

public:
    Signal<> clicked;
    Signal<bool> toggled;

private:
    bool m_checkable;
    bool m_checked;

    String m_string;
    float m_line_spacing;
};

} // namespace ae::gui

#endif // AE_GUI_BUTTON_H
