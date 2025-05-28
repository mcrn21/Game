#include "input_text.h"
#include "gui_theme.h"

InputText::InputText() {}

void InputText::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawButtonBg(vec2{0.0f},
                           getSize(),
                           GuiTheme::palette.button_bg,
                           GuiTheme::palette.button_border,
                           batch_2d);

    auto text_size = getFont()->getFontPage(getFontPixelSize())->getTextSize(getString());
    batch_2d.drawText(getString(),
                      (getSize() - text_size) / 2.0f,
                      GuiTheme::palette.button_text,
                      getFont(),
                      getFontPixelSize());
}
