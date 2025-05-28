#ifndef SETTINGS_FRAME_H
#define SETTINGS_FRAME_H

#include <ae/gui/control.h>

using namespace ae;

class SettingsFrame : public gui::Control
{
public:
    SettingsFrame();
    ~SettingsFrame() = default;

    void onCreated();

protected:
    void drawControl(Batch2D &batch_2d);
};

#endif // SETTINGS_FRAME_H
