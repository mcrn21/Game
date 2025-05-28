#ifndef BUTTON_H
#define BUTTON_H

#include <ae/animation.h>
#include <ae/graphics/core/font.h>
#include <ae/gui/button_base.h>

using namespace ae;

class Button : public gui::ButtonBase
{
public:
    Button();
    ~Button() = default;

    const String &getString() const;
    void setString(const String &string);

    void onStateChanged(int32_t);
    void onSizeChanged(const vec2 &);

    void onCursorEnter();
    void onCursorLeave();

protected:
    void drawControl(Batch2D &batch_2d);

private:
    String m_string;

    std::shared_ptr<FloatAnimation> m_enter_animation;
    std::shared_ptr<FloatAnimation> m_leave_animation;
    Color m_bg_color;
    Color m_border_color;
    float m_anim_time;
};

#endif // BUTTON_H
