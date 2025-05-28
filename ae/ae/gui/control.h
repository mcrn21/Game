#ifndef AE_GUI_CONTROL_H
#define AE_GUI_CONTROL_H

#include "../graphics/core/batch_2d.h"
#include "../graphics/core/shader.h"
#include "../system/memory.h"
#include "../window/input.h"

#include <glm/glm.hpp>

#include <vector>

using namespace glm;

namespace ae {
class Gui;
}

namespace ae::gui {

class Control : public EnableSharedFromThis<Control>
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

    template<typename T>
    struct ControlDeleter
    {
        void operator()(T *ptr) const noexcept
        {
            ptr->m_destroyed = true;
            // Удаляем из потомков родителя, если он установлен
            auto p = ptr->m_parent.lock();
            if (p && !p->m_destroyed) {
                p->m_children.erase(std::remove(p->m_children.begin(),
                                                p->m_children.end(),
                                                ptr->sharedFromThis()),
                                    p->m_children.end());
            }

            ptr->~T();
        }
    };

    template<typename T, typename... Args>
    static SharedPtr<T> create(Args &&...args)
    {
        auto c = SharedPtr<T>::template create<ControlDeleter<T>>(std::forward<Args>(args)...);
        c->onCreated();
        return c;
    }

    Control();
    virtual ~Control();

    Gui *getGui() const;

    SharedPtr<Control> getParent() const;
    void setParent(const SharedPtr<Control> &parent);

    const std::vector<SharedPtr<Control>> &getChildren() const;

    int32_t getState() const;
    void setState(int32_t state);

    bool isEnable() const;
    void setEnable(bool enable);

    bool isVisible() const;
    void setVisible(bool visible);

    const vec2 &getSize() const;
    void setSize(const vec2 &size);

    const vec2 &getPosition() const;
    void setPosition(const vec2 &position);

    const mat4 &getTransform() const;

    SharedPtr<Control> findContolAtPos(const vec2 &pos, vec2 *control_global_position = nullptr);
    bool contains(const vec2 &pos) const;

    float getFontPixelSize() const;
    void setFontPixelSize(float pixel_size);

    const std::shared_ptr<Font> &getFont() const;
    void setFont(const std::shared_ptr<Font> &font);

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

    virtual void onKeyPressed(KeyCode code);
    virtual void onKeyHeld(KeyCode code);
    virtual void onKeyReleased(KeyCode code);
    virtual void onCodepointInputed(uint32_t codepoint);

protected:
    virtual void drawControl(Batch2D &batch_2d);

private:
    void updateGui(Gui *gui);

private:
    Gui *m_gui;

    std::vector<SharedPtr<Control>> m_children;
    WeakPtr<Control> m_parent;

    int32_t m_state;

    bool m_enable;
    bool m_visible;

    vec2 m_size;
    vec2 m_position;
    mutable mat4 m_transform;
    mutable bool m_transform_dirty;

    float m_font_pixel_size;
    std::shared_ptr<Font> m_font;

    mutable Batch2D m_batch_2d;
    mutable bool m_draw_dirty;

    bool m_destroyed;
};

} // namespace ae::gui

#endif // AE_GUI_CONTROL_H
