#include "frame.h"
#include "gui_theme.h"

Frame::Frame()
    : Control{}
{}

void Frame::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawDialogBg(vec2{0.0f}, getSize(), batch_2d);
}
