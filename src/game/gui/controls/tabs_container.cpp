#include "tabs_container.h"

TabsContainer::TabsContainer(EngineContext &engine_context)
    : gui::Control{engine_context}
    , m_color{Color::white}
{
    m_buttons_group.checked.connect(
        [this](int32_t current, int32_t prev) { tabActivated(current); });
}

void TabsContainer::setColor(const Color &color)
{
    m_color = color;
    for (auto &button : m_tab_buttons)
        button->setColor(color);
}

void TabsContainer::setTabs(const std::vector<String> &tabs)
{
    clear();

    int32_t i = 0;
    for (const auto &tab_name : tabs) {
        auto tab_button = gui::Control::create<TabButton>(getEngineContext());
        tab_button->setParent(sharedFromThis());

        tab_button->setFont(getFont());
        tab_button->setFontPixelHeight(getFontPixelHeight());
        tab_button->setColor(m_color);
        tab_button->setPadding(vec4{getFontPixelHeight() * 0.5f});
        tab_button->setString(tab_name);
        ++i;

        m_tab_buttons.push_back(tab_button);
        m_buttons_group.addButton(tab_button);
    }

    updateGeometry();
}

void TabsContainer::clear()
{
    for (auto &tab_button : m_tab_buttons)
        tab_button->setParent(nullptr);
    m_tab_buttons.clear();
    m_buttons_group.clear();
}

void TabsContainer::onSizeChanged(const vec2 &)
{
    updateGeometry();
}

void TabsContainer::updateGeometry()
{
    float x_offset = 0.0f;

    float button_height = getFontPixelHeight() * 1.2f;
    float button_spacing = getFontPixelHeight() * 0.6f;

    for (auto &tab_button : m_tab_buttons) {
        vec2 size = tab_button->getImplicitSize();
        size.y = button_height;
        tab_button->setSize(size);
        tab_button->setPosition(vec2{x_offset, 0.0f});
        x_offset += size.x + button_spacing;
    }

    setImplicitSize(vec2{x_offset, button_height});
}
