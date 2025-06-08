#ifndef AE_GUI_LABEL_H
#define AE_GUI_LABEL_H

#include "../system/string.h"
#include "control.h"

namespace ae::gui {

class Label : public Control
{
public:
    Label(EngineContext &engine_context);
    ~Label() = default;

    const String &getString() const;
    void setString(const String &string);

    float getLineSpacing() const;
    void setLineSpacing(float line_spacing);

    const Color &getColor() const;
    void setColor(const Color &color);

protected:
    void drawControl(Batch2D &batch_2d);
    void updateImplicitSize();

private:
    String m_string;
    float m_line_spacing;
    Color m_color;
};

} // namespace ae::gui

#endif // AE_GUI_LABEL_H
