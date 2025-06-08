#include "fake_terminal_button.h"

#include "../style.h"

#include <ae/animation_manager.h>
#include <ae/engine_context.h>
#include <ae/gui/gui.h>

FakeTerminalButton::FakeTerminalButton(EngineContext &engine_context)
    : gui::ButtonBase{engine_context}
    , m_bg_color{Color::fromInt(100, 100, 100, 255)}
    , m_bg_fill{0.0f}
    , m_current_string_color{Color::white}
    , m_click_sound{createShared<Sound>()}
{
    m_enter_animation = createShared<FloatAnimation>(
        0.0f,
        1.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            m_current_string_color.setColor(
                glm::mix(m_current_string_color.getColor(), m_string_hovered_color.getColor(), v));
            repaint();
        },
        Easing::easeInOutExpo);

    m_leave_animation = createShared<FloatAnimation>(
        1.0f,
        0.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            m_current_string_color.setColor(glm::mix(m_current_string_color.getColor(),
                                                     m_string_default_color.getColor(),
                                                     1.0f - v));
            repaint();
        },
        Easing::easeInOutExpo);
}

FakeTerminalButton::~FakeTerminalButton()
{
    if (m_click_sound->getStatus() == SoundSource::PLAYING)
        getEngineContext().deleteLater(m_click_sound->getRemaining(), m_click_sound);
}

void FakeTerminalButton::setColor(const Color &color)
{
    m_bg_color = color;

    m_string_default_color = Style::Palette::getInverseOn(color);
    m_string_hovered_color = Style::Palette::getOn(color);

    m_current_string_color = m_string_default_color;

    repaint();
}

void FakeTerminalButton::setClickSound(const s_ptr<SoundBuffer> &sound_buffer)
{
    m_click_sound->setSoundBuffer(sound_buffer);
}

void FakeTerminalButton::onCursorEnter()
{
    m_leave_animation->stop();
    m_enter_animation->setFrom(m_bg_fill);
    getEngineContext().getAnimationManager()->run(m_enter_animation);
}

void FakeTerminalButton::onCursorLeave()
{
    m_enter_animation->stop();
    m_leave_animation->setFrom(m_bg_fill);
    getEngineContext().getAnimationManager()->run(m_leave_animation);
}

void FakeTerminalButton::onButtonPressed(ButtonCode button)
{
    if (button == ButtonCode::BUTTON_0)
        m_click_sound->play();
}

void FakeTerminalButton::drawControl(Batch2D &batch_2d)
{
    const auto &size = getSize();
    float left_border_thickness = std::min(size.x, size.y) * 0.2f;

    Style::drawFakeTerminalButtonBg(vec2{0.0f},
                                    size,
                                    m_bg_color.darker(),
                                    m_bg_color,
                                    m_bg_fill,
                                    left_border_thickness,
                                    left_border_thickness * 0.8f,
                                    batch_2d);

    auto *font_page = getFont()->getFontPage(getFontPixelHeight());
    vec2 pos;
    pos.x = left_border_thickness * 2.4f;
    pos.y = (getSize().y - (font_page->getAscent() - font_page->getDescent())) / 2.0f;
    batch_2d.drawText(getString(),
                      pos,
                      m_current_string_color,
                      getFont(),
                      getFontPixelHeight(),
                      getLineSpacing());
}
