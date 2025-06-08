#include "tab_button.h"
#include "../style.h"

#include <ae/animation_manager.h>
#include <ae/engine_context.h>
#include <ae/gui/gui.h>

TabButton::TabButton(EngineContext &engine_context)
    : gui::ButtonBase{engine_context}
    , m_cursor_entered{false}
    , m_border_color{Color::white}
    , m_bg_default_color{Color::transparent}
    , m_bg_hovered_color{Color::white}
    , m_string_default_color{Color::white}
    , m_string_hovered_color{Color::black}
    , m_anim_time{0.0f}
    , m_current_bg_color{Color::transparent}
    , m_current_string_color{Color::white}
{
    m_enter_animation = createShared<FloatAnimation>(
        0.0f,
        1.0f,
        seconds(0.2f),
        [&](float v) {
            m_anim_time = v;
            m_current_bg_color.setColor(
                glm::mix(m_current_bg_color.getColor(), m_bg_hovered_color.getColor(), v));
            m_current_string_color.setColor(
                glm::mix(m_current_string_color.getColor(), m_string_hovered_color.getColor(), v));
            repaint();
        },
        Easing::linear);

    m_leave_animation = createShared<FloatAnimation>(
        1.0f,
        0.0f,
        seconds(0.2f),
        [&](float v) {
            m_anim_time = v;
            m_current_bg_color.setColor(
                glm::mix(m_current_bg_color.getColor(), m_bg_default_color.getColor(), 1.0f - v));
            m_current_string_color.setColor(glm::mix(m_current_string_color.getColor(),
                                                     m_string_default_color.getColor(),
                                                     1.0f - v));
            repaint();
        },
        Easing::linear);

    toggled.connect([this](bool toggle) {
        if (toggle) {
            if (!m_cursor_entered) {
                m_leave_animation->stop();
                m_enter_animation->setFrom(m_anim_time);
                getEngineContext().getAnimationManager()->run(m_enter_animation);
            }
        } else {
            if (!m_cursor_entered) {
                m_enter_animation->stop();
                m_leave_animation->setFrom(m_anim_time);
                getEngineContext().getAnimationManager()->run(m_leave_animation);
            }
        }
    });
}

void TabButton::setColor(const Color &color)
{
    m_border_color = color;
    m_bg_default_color = Color::transparent;
    m_bg_hovered_color = color;
    m_string_default_color = Style::Palette::getInverseOn(color);
    m_string_hovered_color = Style::Palette::getOn(color);

    m_current_bg_color = m_bg_default_color;
    m_current_string_color = m_string_default_color;
}

void TabButton::onCursorEnter()
{
    m_cursor_entered = true;

    if (isChecked())
        return;

    m_leave_animation->stop();
    m_enter_animation->setFrom(m_anim_time);
    getEngineContext().getAnimationManager()->run(m_enter_animation);
}

void TabButton::onCursorLeave()
{
    m_cursor_entered = false;

    if (isChecked())
        return;

    m_enter_animation->stop();
    m_leave_animation->setFrom(m_anim_time);
    getEngineContext().getAnimationManager()->run(m_leave_animation);
}

void TabButton::drawControl(Batch2D &batch_2d)
{
    const auto &size = getSize();
    float corner_thickness = std::min(size.x, size.y) * 0.1f;
    float corner_width = corner_thickness * 2.0f;

    Style::drawTabButtonBg(vec2{0.0f},
                           getSize(),
                           m_current_bg_color,
                           m_border_color,
                           corner_thickness,
                           corner_width,
                           batch_2d);
    auto text_size
        = getFont()->getFontPage(getFontPixelHeight())->getTextSize(getString(), getLineSpacing());
    batch_2d.drawText(getString(),
                      (getSize() - text_size) / 2.0f,
                      m_current_string_color,
                      getFont(),
                      getFontPixelHeight(),
                      getLineSpacing());
}
