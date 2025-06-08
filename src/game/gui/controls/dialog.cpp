#include "dialog.h"
#include "../gui_theme.h"

#include <ae/gui/gui.h>

Dialog::Dialog(EngineContext &engine_context)
    : gui::Control{engine_context}
    , m_center{0.0f}
{
    setFontPixelHeight(GuiTheme::metrics.dialog_font_pixel_size);
}

void Dialog::setCenter(const vec2 &center)
{
    m_center = center;
    updateGeometry();
}

const String &Dialog::getString() const
{
    return m_string;
}

void Dialog::setString(const String &string)
{
    m_string = string;
    updateGeometry();
}

void Dialog::setAcceptString(const String &string)
{
    m_accept_button->setString(string);
    updateGeometry();
}

void Dialog::setCancelString(const String &string)
{
    m_cancel_button->setString(string);
    updateGeometry();
}

void Dialog::onCreated()
{
    m_accept_button = gui::Control::create<Button>(getEngineContext());
    m_accept_button->setParent(sharedFromThis());
    m_accept_button->clicked.connect([this]() { accepted(); });

    m_cancel_button = gui::Control::create<Button>(getEngineContext());
    m_cancel_button->setParent(sharedFromThis());
    m_cancel_button->clicked.connect([this]() { canceled(); });
}

void Dialog::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawDialogBg(vec2{0.0f}, getSize(), batch_2d);
    batch_2d.drawText(m_string,
                      vec2{GuiTheme::metrics.dialog_text_padding},
                      Color::white,
                      getFont(),
                      getFontPixelHeight());
}

void Dialog::updateGeometry()
{
    auto text_size = getFont()->getFontPage(getFontPixelHeight())->getTextSize(m_string);
    text_size.x += GuiTheme::metrics.dialog_text_padding * 2.0f;
    text_size.y += GuiTheme::metrics.dialog_text_padding * 2.0f
                   + GuiTheme::metrics.dialog_button_height
                   + GuiTheme::metrics.dialog_buttons_bottom_offset;

    setSize(text_size);
    setPosition(m_center - getSize() / 2.0f);

    vec2 button_padding = GuiTheme::metrics.dialog_button_padding;

    m_accept_button->setSize(vec2{m_accept_button->getFont()
                                          ->getFontPage(m_accept_button->getFontPixelHeight())
                                          ->getTextSize(m_accept_button->getString())
                                          .x
                                      + button_padding.x * 2.0f,
                                  GuiTheme::metrics.dialog_button_height + button_padding.y * 2.0f});

    m_cancel_button->setSize(vec2{m_cancel_button->getFont()
                                          ->getFontPage(m_cancel_button->getFontPixelHeight())
                                          ->getTextSize(m_cancel_button->getString())
                                          .x
                                      + button_padding.x * 2.0f,
                                  GuiTheme::metrics.dialog_button_height + button_padding.y * 2.0f});

    float buttons_width = m_accept_button->getSize().x + m_cancel_button->getSize().x
                          + GuiTheme::metrics.dialog_buttons_spacing;

    m_accept_button->setPosition(vec2{(getSize().x - buttons_width) / 2.0f,
                                      getSize().y - m_accept_button->getSize().y
                                          - GuiTheme::metrics.dialog_buttons_bottom_offset});

    m_cancel_button->setPosition(vec2{(getSize().x - buttons_width) / 2.0f
                                          + m_accept_button->getSize().x
                                          + GuiTheme::metrics.dialog_buttons_spacing,
                                      getSize().y - m_cancel_button->getSize().y
                                          - GuiTheme::metrics.dialog_buttons_bottom_offset});

    repaint();
}
