#include "button.h"
#include "gui_theme.h"

#include <ae/gui/gui.h>

#include <spdlog/spdlog.h>

Button::Button()
    : m_anim_time{0.0f}
{
    setFontPixelSize(GuiTheme::metrics.button_font_pixel_size);

    m_bg_color = GuiTheme::palette.button_bg;
    m_border_color = GuiTheme::palette.button_border;

    m_enter_animation = std::make_shared<FloatAnimation>(
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

    m_leave_animation = std::make_shared<FloatAnimation>(
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

void Button::onStateChanged(int32_t)
{
    repaint();
}

void Button::onSizeChanged(const vec2 &)
{
    repaint();
}

void Button::onCursorEnter()
{
    m_leave_animation->stop();
    m_enter_animation->setFrom(m_anim_time);
    m_enter_animation->start();
}

void Button::onCursorLeave()
{
    m_enter_animation->stop();
    m_leave_animation->setFrom(m_anim_time);
    m_leave_animation->start();
}

void Button::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawButtonBg(vec2{0.0f}, getSize(), m_bg_color, m_border_color, batch_2d);

    auto text_size = getFont()->getFontPage(getFontPixelSize())->getTextSize(m_string);
    batch_2d.drawText(m_string,
                      (getSize() - text_size) / 2.0f,
                      GuiTheme::palette.button_text,
                      getFont(),
                      getFontPixelSize());
}
