#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "controls/tabs_container.h"

#include <ae/gui/control.h>

using namespace ae;

class SettingsDialog : public gui::Control
{
public:
    SettingsDialog(EngineContext &engine_context);
    ~SettingsDialog() = default;

    void onCreated();
    void onSizeChanged(const vec2 &);

protected:
    void drawControl(Batch2D &batch_2d);

private:
    void setActiveTab(int32_t index);
    void updateGeometry();

    s_ptr<gui::Control> createVideoTab();
    s_ptr<gui::Control> createAudioTab();
    s_ptr<gui::Control> createControlsTab();

private:
    s_ptr<TabsContainer> m_tabs_container;
    std::vector<s_ptr<gui::Control>> m_tabs;
};

#endif // SETTINGS_DIALOG_H
