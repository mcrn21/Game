#ifndef LOAD_LEVEL_GUI_H
#define LOAD_LEVEL_GUI_H

#include "controls/fake_terminal.h"
#include "controls/progress_bar.h"

#include <ae/gui/control.h>
#include <ae/gui/label.h>

using namespace ae;

class LoadLevelGui : public gui::Control
{
public:
    LoadLevelGui(EngineContext &engine_context);
    ~LoadLevelGui() = default;

    void setProgress(float progress);
    void addString(const String &string);

    void onCreated();
    void onSizeChanged(const vec2 &size);

private:
    s_ptr<FakeTerminal> m_fake_terminal;
    s_ptr<ProgressBar> m_progress_bar;
};

#endif // LOAD_LEVEL_GUI_H
