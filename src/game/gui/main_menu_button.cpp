#include "main_menu_button.h"
#include "gui_theme.h"

#include <ae/gui/gui.h>

MainMenuButton::MainMenuButton()
    : gui::ButtonBase{}
    , m_bg_fill{0.0f}
{
    m_font = Gui::getDefaultFont();

    m_a1 = std::make_shared<FloatAnimation>(
        0.0f,
        1.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            repaint();
        },
        Easing::easeInOutExpo);

    m_a2 = std::make_shared<FloatAnimation>(
        1.0f,
        0.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            repaint();
        },
        Easing::easeInOutExpo);
}

const std::shared_ptr<Font> &MainMenuButton::getFont() const
{
    return m_font;
}

void MainMenuButton::setFont(const std::shared_ptr<Font> &font)
{
    m_font = font;
}

const String &MainMenuButton::getString() const
{
    return m_string;
}

void MainMenuButton::setString(const String &string)
{
    m_string = string;
    repaint();
}

void MainMenuButton::onStateChanged(int32_t)
{
    repaint();
}

void MainMenuButton::onSizeChanged(const vec2 &)
{
    repaint();
}

void MainMenuButton::onCursorEnter()
{
    m_a2->stop();
    m_a1->setFrom(m_bg_fill);
    m_a1->start();
}

void MainMenuButton::onCursorLeave()
{
    m_a1->stop();
    m_a2->setFrom(m_bg_fill);
    m_a2->start();
}

void MainMenuButton::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawMainMenuButtonBg(vec2{0.0f}, getSize(), m_bg_fill, batch_2d);
    batch_2d.drawText(m_string,
                      vec2{GuiTheme::main_menu_button_text_left_offset,
                           (getSize().y - m_font->getPixelHeight()) / 2.0f},
                      Color::white,
                      m_font);
}
