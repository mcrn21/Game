#ifndef FIRST_STATE_GUI_H
#define FIRST_STATE_GUI_H

#include "controls/fake_terminal.h"
#include "controls/progress_bar.h"

#include <ae/graphics/core/texture.h>
#include <ae/gui/control.h>

using namespace ae;

class FirstStateGui : public gui::Control
{
public:
    FirstStateGui(EngineContext &engine_context);
    ~FirstStateGui() = default;

    void setSplashTexture(const s_ptr<Texture> &splash_texture);
    void createFakeTerminal();
    void setProgress(float progress);
    void appendStringToCmdOutput(const String &string);

    void onSizeChanged(const vec2 &size);

protected:
    void drawControl(Batch2D &batch_2d);

private:
    s_ptr<Texture> m_splash_texture;
    s_ptr<FakeTerminal> m_fake_terminal;
    s_ptr<ProgressBar> m_progress_bar;
};

#endif // FIRST_STATE_GUI_H
