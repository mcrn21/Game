#ifndef AE_GUI_PROGRESS_BAR_BASE_H
#define AE_GUI_PROGRESS_BAR_BASE_H

#include "control.h"

namespace ae::gui {

class ProgressBarBase : public Control
{
public:
    ProgressBarBase(EngineContext &engine_context);
    ~ProgressBarBase() = default;

    float getProgress() const;
    void setProgress(float progress);

private:
    float m_progress;
};

} // namespace ae::gui

#endif // AE_GUI_PROGRESS_BAR_BASE_H
