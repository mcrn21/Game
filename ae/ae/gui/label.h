#ifndef AE_GUI_LABEL_H
#define AE_GUI_LABEL_H

#include "../system/string.h"
#include "control.h"

#include "../../3rd/sigslot/signal.hpp"

namespace ae::gui {

class Label : public Control
{
public:
    Label();
    ~Label() = default;

    const String &getString() const;
    void setString(const String &string);

    void onButtonPressed(ButtonCode button);

public:
    sigslot::signal<> clicked;

protected:
    void drawControl(Batch2D &batch_2d);

private:
    String m_string;
};

} // namespace ae::gui

#endif // AE_GUI_LABEL_H
