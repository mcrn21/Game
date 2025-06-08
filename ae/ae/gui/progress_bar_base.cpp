#include "progress_bar_base.h"

#include <algorithm>

namespace ae::gui {

ProgressBarBase::ProgressBarBase(EngineContext &engine_context)
    : Control{engine_context}
    , m_progress{0.0f}
{}

float ProgressBarBase::getProgress() const
{
    return m_progress;
}

void ProgressBarBase::setProgress(float progress)
{
    m_progress = std::clamp(progress, 0.0f, 1.0f);
    repaint();
}

} // namespace ae::gui
