#ifndef AE_GUI_CONTROL_H
#define AE_GUI_CONTROL_H

#include "../graphics/core/batch_2d.h"
#include "../graphics/core/shader.h"
#include "../window/input.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

using namespace glm;

namespace ae {
class Gui;
}

namespace ae::gui {

class Control : public std::enable_shared_from_this<Control>
{
    friend class ::ae::Gui;

public:
    enum State {
        DISABLED = 0x0001,
        DEFAULT = 0x0002,
        HOVERED = 0x0004,
        FOCUSED = 0x0008,
        PRESSED = 0x0010
    };

    template<typename T, typename... Args>
    static std::shared_ptr<T> create(Args &&...args)
    {
        auto c = std::make_shared<T>(std::forward<Args>(args)...);
        c->onCreated();
        return c;
    }

    Control();
    virtual ~Control();

    Gui *getGui() const;

    std::shared_ptr<Control> getParent() const;
    void setParent(const std::shared_ptr<Control> &parent);

    const std::vector<std::shared_ptr<Control>> &getChildren() const;

    int32_t getState() const;
    void setState(int32_t state);

    bool isVisible() const;
    void setVisible(bool visible);

    const vec2 &getSize() const;
    void setSize(const vec2 &size);

    const vec2 &getPosition() const;
    void setPosition(const vec2 &position);

    const mat4 &getTransform() const;

    std::shared_ptr<Control> findContolAtPos(const vec2 &pos,
                                             vec2 *control_global_position = nullptr);
    bool contains(const vec2 &pos) const;

    void repaint();
    virtual void draw(Shader *shader, const mat4 &transform = mat4{1.0f}) const;

    // Events
    virtual void onCreated();

    virtual void onStateChanged(int32_t state);
    virtual void onSizeChanged(const vec2 &size);

    virtual void onCursorEnter();
    virtual void onCursorLeave();
    virtual void onCursorMoved(const vec2 &position);

    virtual void onButtonPressed(ButtonCode button);
    virtual void onButtonReleased(ButtonCode button);

protected:
    virtual void drawControl(Batch2D &batch_2d);

private:
    void updateGui(Gui *gui);

private:
    Gui *m_gui;

    std::vector<std::shared_ptr<Control>> m_children;
    std::weak_ptr<Control> m_parent;

    int32_t m_state;

    bool m_visible;

    vec2 m_size;
    vec2 m_position;
    mutable mat4 m_transform;
    mutable bool m_transform_dirty;

    mutable Batch2D m_batch_2d;
    mutable bool m_draw_dirty;
};

} // namespace ae::gui

#endif // AE_GUI_CONTROL_H
