#ifndef BUTTON_H
#define BUTTON_H

#include <ae/graphics/core/font.h>
#include <ae/gui/button_base.h>

using namespace ae;

class Button : public gui::ButtonBase
{
public:
    Button();
    ~Button() = default;

    const std::shared_ptr<Font> &getFont() const;
    void setFont(const std::shared_ptr<Font> &font);

    const String &getString() const;
    void setString(const String &string);

    void onStateChanged(int32_t);
    void onSizeChanged(const vec2 &);

    void onCursorEnter();
    void onCursorLeave();

protected:
    void drawControl(Batch2D &batch_2d);

private:
    std::shared_ptr<Font> m_font;
    String m_string;
};

#endif // BUTTON_H
