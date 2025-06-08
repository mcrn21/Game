#ifndef TABS_CONTAINER_H
#define TABS_CONTAINER_H

#include "tab_button.h"

#include <ae/gui/buttons_group.h>
#include <ae/gui/control.h>

#include <vector>

using namespace ae;

class TabsContainer : public gui::Control
{
public:
    TabsContainer(EngineContext &engine_context);
    ~TabsContainer() = default;

    void setColor(const Color &color);

    void setTabs(const std::vector<String> &tabs);
    void clear();

    void onSizeChanged(const vec2 &);

private:
    void updateGeometry();

public:
    Signal<int32_t> tabActivated;

private:
    Color m_color;

    std::vector<s_ptr<TabButton>> m_tab_buttons;
    gui::ButtonsGroup m_buttons_group;
};

#endif // TABS_CONTAINER_H
