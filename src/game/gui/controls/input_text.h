#ifndef INPUT_TEXT_H
#define INPUT_TEXT_H

#include <ae/gui/input_text_base.h>

using namespace ae;

class InputText : public gui::InputTextBase
{
public:
    InputText(EngineContext &engine_context);
    ~InputText() = default;

protected:
    void drawControl(Batch2D &batch_2d);
};

#endif // INPUT_TEXT_H
