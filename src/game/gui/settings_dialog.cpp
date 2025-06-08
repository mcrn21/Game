#include "settings_dialog.h"
#include "controls/input_text.h"
#include "controls/tabs_container.h"
#include "style.h"

SettingsDialog::SettingsDialog(EngineContext &engine_context)
    : gui::Control{engine_context}
{}

void SettingsDialog::onCreated()
{
    m_tabs_container = gui::Control::create<TabsContainer>(getEngineContext());
    m_tabs_container->setParent(sharedFromThis());

    m_tabs_container->setColor(Style::Palette::tertiary);
    m_tabs_container->setFontPixelHeight(38.0f);

    m_tabs_container->tabActivated.connect([&](int32_t tab) { setActiveTab(tab); });

    m_tabs.push_back(createVideoTab());
    m_tabs.push_back(createAudioTab());
    m_tabs.push_back(createControlsTab());

    m_tabs_container->setTabs({"Video", "Audio", "Controls"});
    m_tabs_container->setSize(m_tabs_container->getImplicitSize());

    updateGeometry();
}

void SettingsDialog::onSizeChanged(const vec2 &)
{
    updateGeometry();
}

void SettingsDialog::drawControl(Batch2D &batch_2d)
{
    // GuiTheme::drawDialogBg(vec2{0.0f}, getSize(), batch_2d);
}

void SettingsDialog::setActiveTab(int32_t index)
{
    if (m_tabs.empty() || index < 0)
        return;

    if (index > m_tabs.size())
        index = m_tabs.size() - 1;

    for (auto &tab : m_tabs)
        tab->setParent(nullptr);

    m_tabs[index]->setParent(sharedFromThis());
}

void SettingsDialog::updateGeometry()
{
    auto size = getSize();
    size.y -= m_tabs_container->getSize().y;

    for (auto &tab : m_tabs) {
        tab->setPosition(vec2{0.0f, m_tabs_container->getSize().y});
        tab->setSize(size);
    }
}

s_ptr<gui::Control> SettingsDialog::createVideoTab()
{
    auto tab = gui::Control::create<gui::Control>(getEngineContext());

    auto input_text = gui::Control::create<InputText>(getEngineContext());
    input_text->setParent(tab);
    input_text->setSize(vec2{200.0f, 64.0f});
    input_text->setPosition(vec2{50.0f});

    return tab;
}

s_ptr<gui::Control> SettingsDialog::createAudioTab()
{
    auto tab = gui::Control::create<gui::Control>(getEngineContext());

    auto input_text = gui::Control::create<InputText>(getEngineContext());
    input_text->setParent(tab);
    input_text->setSize(vec2{200.0f, 64.0f});
    input_text->setPosition(vec2{150.0f});

    return tab;
}

s_ptr<gui::Control> SettingsDialog::createControlsTab()
{
    auto tab = gui::Control::create<gui::Control>(getEngineContext());

    auto input_text = gui::Control::create<InputText>(getEngineContext());
    input_text->setParent(tab);
    input_text->setSize(vec2{200.0f, 64.0f});
    input_text->setPosition(vec2{250.0f});

    return tab;
}
