#include "progress_bar.h"
#include "../style.h"

ProgressBar::ProgressBar(EngineContext &engine_context)
    : gui::ProgressBarBase{engine_context}
    , m_color{Color::white}
{}

void ProgressBar::setColor(const Color &color)
{
    m_color = color;
}

void ProgressBar::drawControl(Batch2D &batch_2d)
{
    const auto &size = getSize();
    float corner_thickness = std::min(size.x, size.y) * 0.14f;
    float corner_width = corner_thickness * 2.0f;

    Style::drawProgressBarBg(vec2{0.0f},
                             getSize(),
                             m_color,
                             m_color,
                             corner_thickness,
                             corner_width,
                             getProgress(),
                             batch_2d);
}
