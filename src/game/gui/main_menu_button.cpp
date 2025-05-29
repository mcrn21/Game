#include "main_menu_button.h"
#include "gui_theme.h"

#include <ae/gui/gui.h>

MainMenuButton::MainMenuButton()
    : gui::ButtonBase{}
    , m_bg_fill{0.0f}
{
    setFontPixelSize(GuiTheme::metrics.main_menu_button_font_pixel_size);

    m_a1 = createShared<FloatAnimation>(
        0.0f,
        1.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            repaint();
        },
        Easing::easeInOutExpo);

    m_a2 = createShared<FloatAnimation>(
        1.0f,
        0.0f,
        seconds(0.1f),
        [&](float v) {
            m_bg_fill = v;
            repaint();
        },
        Easing::easeInOutExpo);
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
    GuiTheme::drawMainMenuButtonBg(vec2{0.0f},
                                   getSize(),
                                   GuiTheme::palette.main_menu_button_hovered_bg,
                                   GuiTheme::palette.main_menu_button_border,
                                   m_bg_fill,
                                   batch_2d);

    auto *font_page = getFont()->getFontPage(getFontPixelSize());
    vec2 pos;
    pos.x = GuiTheme::metrics.main_menu_button_border_thickness * 2.2f;
    pos.y = (getSize().y - (font_page->getAscent() - font_page->getDescent())) / 2.0f;
    batch_2d.drawText(m_string,
                      pos,
                      GuiTheme::palette.main_menu_button_text,
                      getFont(),
                      getFontPixelSize());
}
