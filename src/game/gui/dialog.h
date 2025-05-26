#ifndef DIALOG_H
#define DIALOG_H

#include "button.h"
#include "frame.h"

class Dialog : public Frame
{
public:
    Dialog();
    ~Dialog() = default;

    void setCenter(const vec2 &center);

    const std::shared_ptr<Font> &getFont() const;
    void setFont(const std::shared_ptr<Font> &font);

    const String &getString() const;
    void setString(const String &string);

    void setAcceptString(const String &string);
    void setCancelString(const String &string);

    void onCreated();

protected:
    void drawControl(Batch2D &batch_2d);

private:
    void updateDialogPosition();
    void updateButtonPositions();

public:
    sigslot::signal<> accepted;
    sigslot::signal<> canceled;

private:
    vec2 m_center;

    std::shared_ptr<Font> m_font;
    String m_string;

    std::shared_ptr<Button> m_accept_button;
    std::shared_ptr<Button> m_cancel_button;
};

#endif // DIALOG_H
