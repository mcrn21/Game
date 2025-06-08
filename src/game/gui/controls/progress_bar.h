#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <ae/gui/progress_bar_base.h>

using namespace ae;

class ProgressBar : public gui::ProgressBarBase
{
public:
    ProgressBar(EngineContext &engine_context);
    ~ProgressBar() = default;

protected:
    void drawControl(Batch2D &batch_2d);
};

#endif // PROGRESS_BAR_H
