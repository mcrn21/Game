#include "dialog.h"

#include <ae/gui/gui.h>

Dialog::Dialog()
    : m_center{0.0f}
{
    m_font = Gui::getDefaultFont();
}

void Dialog::setCenter(const vec2 &center)
{
    m_center = center;
    updateDialogPosition();
}

const std::shared_ptr<Font> &Dialog::getFont() const
{
    return m_font;
}

void Dialog::setFont(const std::shared_ptr<Font> &font)
{
    m_font = font;
}

const String &Dialog::getString() const
{
    return m_string;
}

void Dialog::setString(const String &string)
{
    m_string = string;
    updateDialogPosition();
}

void Dialog::setAcceptString(const String &string)
{
    m_accept_button->setString(string);
    updateButtonPositions();
}

void Dialog::setCancelString(const String &string)
{
    m_cancel_button->setString(string);
    updateButtonPositions();
}

void Dialog::onCreated()
{
    m_accept_button = std::make_shared<Button>();
    m_accept_button->setParent(shared_from_this());

    m_accept_button->clicked.connect([this]() { accepted(); });

    m_cancel_button = std::make_shared<Button>();
    m_cancel_button->setParent(shared_from_this());

    m_cancel_button->clicked.connect([this]() { canceled(); });
}

void Dialog::drawControl(Batch2D &batch_2d)
{
    Frame::drawControl(batch_2d);
    batch_2d.drawText(m_string, vec2{40.0f}, Color::white, m_font);
}

void Dialog::updateDialogPosition()
{
    auto text_size = m_font->getTextSize(m_string);
    text_size.x += 80.0f;
    text_size.y += 160.0f;

    setSize(text_size);
    setPosition(m_center - getSize() / 2.0f);

    updateButtonPositions();
    repaint();
}

void Dialog::updateButtonPositions()
{
    m_accept_button->setSize(
        vec2{m_accept_button->getFont()->getTextSize(m_accept_button->getString()).x + 24.0f,
             48.0f});

    m_cancel_button->setSize(
        vec2{m_cancel_button->getFont()->getTextSize(m_cancel_button->getString()).x + 24.0f,
             48.0f});

    float buttons_width = m_accept_button->getSize().x + m_cancel_button->getSize().x + 24.0f;

    m_accept_button->setPosition(vec2{(getSize().x - buttons_width) / 2.0f,
                                      getSize().y - m_accept_button->getSize().y - 40.0f});

    m_cancel_button->setPosition(
        vec2{(getSize().x - buttons_width) / 2.0f + m_accept_button->getSize().x + 24.0f,
             getSize().y - m_cancel_button->getSize().y - 40.0f});
}
