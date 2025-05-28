#include "settings_frame.h"
#include "gui_theme.h"
#include "input_text.h"

SettingsFrame::SettingsFrame() {}

void SettingsFrame::onCreated()
{
    // Test
    auto input_text = gui::Control::create<InputText>();
    input_text->setParent(sharedFromThis());
    input_text->setSize(vec2{200.0f, 64.0f});
    input_text->setPosition(vec2{50.0f});
}

void SettingsFrame::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawDialogBg(vec2{0.0f}, getSize(), batch_2d);
}
