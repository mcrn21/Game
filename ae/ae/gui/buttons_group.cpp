#include "buttons_group.h"

#include <algorithm>

namespace ae::gui {

ButtonsGroup::ButtonsGroup()
    : m_current{-1}
    , m_prev{-1}
{}

void ButtonsGroup::addButton(const s_ptr<ButtonBase> &button)
{
    int32_t i = m_buttons.size();
    button->clicked.connect([i, button, this]() {
        if (m_current == i)
            return;

        if (m_current != -1)
            m_buttons[m_current]->setChecked(false);

        button->setChecked(true);
        m_prev = m_current;
        m_current = i;

        checked(m_current, m_prev);
    });

    if (m_buttons.empty()) {
        button->setChecked(true);
        button->clicked();
    }

    m_buttons.push_back(button);
}

void ButtonsGroup::removeButton(const s_ptr<ButtonBase> &button)
{
    auto found = std::find(m_buttons.begin(), m_buttons.end(), button);
    if (found == m_buttons.end())
        return;

    int32_t pos = found - m_buttons.begin();
    if (pos == m_current) {
        m_prev = m_current;
        m_current = m_buttons.size() == 1 ? -1 : 0;

        if (m_current != -1)
            m_buttons[m_current]->setChecked(true);

        checked(m_current, m_prev);
    }

    m_buttons.erase(found);
}

int32_t ButtonsGroup::getCurrent() const
{
    return m_current;
}

s_ptr<ButtonBase> ButtonsGroup::getButton(int32_t button_num) const
{
    if (button_num < 0 || button_num > m_buttons.size())
        return nullptr;
    return m_buttons[button_num];
}

void ButtonsGroup::clear()
{
    for (auto &button : m_buttons)
        removeButton(button);
    m_buttons.clear();
}

} // namespace ae::gui
