#ifndef AE_GUI_BUTTON_H
#define AE_GUI_BUTTON_H

#include "../system/string.h"
#include "control.h"

#include "../../3rd/sigslot/signal.hpp"

namespace ae::gui {

class ButtonBase : public Control
{
public:
    ButtonBase();
    ~ButtonBase() = default;

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isChecked() const;

    void onButtonReleased(ButtonCode button);

public:
    sigslot::signal<> clicked;
    sigslot::signal<bool> toggled;

private:
    bool m_checkable;
    bool m_checked;
};

} // namespace ae::gui

#endif // AE_GUI_BUTTON_H
