#ifndef DIALOG_H
#define DIALOG_H

#include "button.h"

#include <ae/gui/control.h>

using namespace ae;

class Dialog : public gui::Control
{
public:
    Dialog(EngineContext &engine_context);
    ~Dialog() = default;

    void setCenter(const vec2 &center);

    const String &getString() const;
    void setString(const String &string);

    void setAcceptString(const String &string);
    void setCancelString(const String &string);

    void onCreated();

protected:
    void drawControl(Batch2D &batch_2d);

private:
    void updateGeometry();

public:
    sigslot::signal<> accepted;
    sigslot::signal<> canceled;

private:
    vec2 m_center;

    String m_string;

    s_ptr<Button> m_accept_button;
    s_ptr<Button> m_cancel_button;
};

#endif // DIALOG_H
