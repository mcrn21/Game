#ifndef TAB_BUTTON_H
#define TAB_BUTTON_H

#include <ae/animation.h>
#include <ae/graphics/core/font.h>
#include <ae/gui/button_base.h>

using namespace ae;

class TabButton : public gui::ButtonBase
{
public:
    TabButton(EngineContext &engine_context);
    ~TabButton() = default;

    void setColor(const Color &color);

    void onCursorEnter();
    void onCursorLeave();

protected:
    void drawControl(Batch2D &batch_2d);

private:
    bool m_cursor_entered;

    Color m_border_color;
    Color m_bg_default_color;
    Color m_bg_hovered_color;
    Color m_string_default_color;
    Color m_string_hovered_color;

    s_ptr<FloatAnimation> m_enter_animation;
    s_ptr<FloatAnimation> m_leave_animation;
    float m_anim_time;
    Color m_current_bg_color;
    Color m_current_string_color;
};

#endif // TAB_BUTTON_H
