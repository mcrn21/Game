#include "button_base.h"

namespace ae::gui {

ButtonBase::ButtonBase()
    : Control{}
    , m_checkable{false}
    , m_checked{false}
{}

bool ButtonBase::isCheckable() const
{
    return m_checkable;
}

void ButtonBase::setCheckable(bool checkable) {}

bool ButtonBase::isChecked() const
{
    return m_checked;
}

void ButtonBase::onButtonReleased(ButtonCode button)
{
    if (button == ButtonCode::BUTTON_0) {
        clicked();
        if (m_checkable) {
            m_checked = !m_checked;
            toggled(m_checked);
        }
    }
}

} // namespace ae::gui
