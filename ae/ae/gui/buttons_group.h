#ifndef AE_GUI_BUTTONS_GROUP_H
#define AE_GUI_BUTTONS_GROUP_H

#include "button_base.h"

#include <vector>

namespace ae::gui {

class ButtonsGroup
{
public:
    ButtonsGroup();
    ~ButtonsGroup() = default;

    void addButton(const s_ptr<ButtonBase> &button);
    void removeButton(const s_ptr<ButtonBase> &button);

    int32_t getCurrent() const;
    s_ptr<ButtonBase> getButton(int32_t button_num) const;

    void clear();

public:
    // current, prev
    Signal<int32_t, int32_t> checked;

private:
    std::vector<s_ptr<ButtonBase>> m_buttons;
    int32_t m_current;
    int32_t m_prev;
};

} // namespace ae::gui

#endif // AE_GUI_BUTTONS_GROUP_H
