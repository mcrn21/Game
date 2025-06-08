#include "progress_bar.h"
#include "../gui_theme.h"

ProgressBar::ProgressBar(EngineContext &engine_context)
    : gui::ProgressBarBase{engine_context}
{}

void ProgressBar::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawProgressBarBg(vec2{0.0f},
                                getSize(),
                                GuiTheme::palette.progress_bar_bg,
                                GuiTheme::palette.progress_bar_border,
                                getProgress(),
                                batch_2d);
}
