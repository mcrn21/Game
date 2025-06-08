#include "input_text.h"
#include "../gui_theme.h"

InputText::InputText(EngineContext &engine_context)
    : gui::InputTextBase{engine_context}
{}

void InputText::drawControl(Batch2D &batch_2d)
{
    GuiTheme::drawButtonBg(vec2{0.0f},
                           getSize(),
                           GuiTheme::palette.button_bg,
                           GuiTheme::palette.button_border,
                           batch_2d);

    auto text_size = getFont()->getFontPage(getFontPixelHeight())->getTextSize(getString());
    batch_2d.drawText(getString(),
                      (getSize() - text_size) / 2.0f,
                      GuiTheme::palette.button_text,
                      getFont(),
                      getFontPixelHeight());
}
