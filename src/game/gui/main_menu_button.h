#ifndef MAIN_MENU_BUTTON_H
#define MAIN_MENU_BUTTON_H

#include <ae/animation.h>
#include <ae/graphics/core/font.h>
#include <ae/gui/button_base.h>
#include <ae/system/string.h>

using namespace ae;

class MainMenuButton : public gui::ButtonBase
{
public:
    MainMenuButton();
    ~MainMenuButton() = default;

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

    std::shared_ptr<FloatAnimation> m_a1;
    std::shared_ptr<FloatAnimation> m_a2;
    float m_bg_fill;
};

#endif // MAIN_MENU_BUTTON_H
