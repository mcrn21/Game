#include "button.h"
#include "../gui_theme.h"

#include <ae/animation_manager.h>
#include <ae/engine_context.h>
#include <ae/gui/gui.h>

Button::Button(EngineContext &engine_context)
    : gui::ButtonBase{engine_context}
    , m_anim_time{0.0f}
{
    setFontPixelHeight(GuiTheme::metrics.button_font_pixel_size);

    m_bg_color = GuiTheme::palette.button_bg;
    m_border_color = GuiTheme::palette.button_border;

    m_enter_animation = createShared<FloatAnimation>(
        0.0f,
        1.0f,
        seconds(0.2f),
        [&](float v) {
            m_anim_time = v;
            m_bg_color.setColor(
                glm::mix(m_bg_color.getColor(), GuiTheme::palette.button_hovered_bg.getColor(), v));
            m_border_color.setColor(glm::mix(m_border_color.getColor(),
                                             GuiTheme::palette.button_hovered_border.getColor(),
                                             v));
            repaint();
        },
        Easing::linear);

    m_leave_animation = createShared<FloatAnimation>(
        1.0f,
        0.0f,
        seconds(0.2f),
        [&](float v) {
            m_anim_time = v;
            m_bg_color.setColor(
                glm::mix(m_bg_color.getColor(), GuiTheme::palette.button_bg.getColor(), 1.0f - v));
            m_border_color.setColor(glm::mix(m_border_color.getColor(),
                                             GuiTheme::palette.button_border.getColor(),
                                             1.0f - v));
            repaint();
        },
        Easing::linear);
}

const String &Button::getString() const
{
    return m_string;
}

void Button::setString(const String &string)
{
    m_string = string;
}

void Button::onCursorEnter()
{
    m_leave_animation->stop();
    m_enter_animation->setFrom(m_anim_time);
    getEngineContext().getAnimationManager()->run(m_enter_animation);
}

void Button::onCursorLeave()
{
    m_enter_animation->stop();
    m_leave_animation->setFrom(m_anim_time);
    getEngineContext().getAnimationManager()->run(m_leave_animation);
}

void Button::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawButtonBg(vec2{0.0f}, getSize(), m_bg_color, m_border_color, batch_2d);

    auto text_size = getFont()->getFontPage(getFontPixelHeight())->getTextSize(m_string);
    batch_2d.drawText(m_string,
                      (getSize() - text_size) / 2.0f,
                      GuiTheme::palette.button_text,
                      getFont(),
                      getFontPixelHeight());
}
