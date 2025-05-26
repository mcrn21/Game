#ifndef AE_GUI_IMAGE_H
#define AE_GUI_IMAGE_H

#include "control.h"

namespace ae::gui {

class Image : public Control
{
public:
    Image();
    ~Image() = default;

    void onStateChanged(int32_t state);

    void onCursorEnter();
    void onCursorLeave();
    void onCursorMoved(const vec2 &position);

    void onButtonPressed(ButtonCode button);
    void onButtonReleased(ButtonCode button);

protected:
    void drawControl(Batch2D &batch_2d);
};

} // namespace ae::gui

#endif // AE_GUI_IMAGE_H
