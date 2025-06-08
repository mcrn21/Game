#ifndef FAKE_TERMINAL_BUTTON_H
#define FAKE_TERMINAL_BUTTON_H

#include <ae/animation.h>
#include <ae/audio/sound.h>
#include <ae/graphics/core/font.h>
#include <ae/gui/button_base.h>
#include <ae/system/string.h>

using namespace ae;

class FakeTerminalButton : public gui::ButtonBase
{
public:
    FakeTerminalButton(EngineContext &engine_context);
    ~FakeTerminalButton();

    void setColor(const Color &color);
    void setClickSound(const s_ptr<SoundBuffer> &sound_buffer);

    void onCursorEnter();
    void onCursorLeave();
    void onButtonPressed(ButtonCode button);

protected:
    void drawControl(Batch2D &batch_2d);

private:
    Color m_bg_color;
    Color m_string_default_color;
    Color m_string_hovered_color;

    s_ptr<Sound> m_click_sound;

    s_ptr<FloatAnimation> m_enter_animation;
    s_ptr<FloatAnimation> m_leave_animation;
    float m_bg_fill;
    Color m_current_string_color;
};
#endif // FAKE_TERMINAL_BUTTON_H
