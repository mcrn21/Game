#ifndef AE_GUI_CONTROL_H
#define AE_GUI_CONTROL_H

#include "../engine_context_object.h"
#include "../graphics/core/batch_2d.h"
#include "../graphics/core/shader.h"
#include "../system/memory.h"
#include "../window/input.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <vector>

using namespace glm;

namespace ae {
class Gui;
}

namespace ae::gui {

class Control : public EngineContextObject, public EnableSharedFromThis<Control>
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
    static s_ptr<T> create(Args &&...args)
    {
        auto c = createShared<T, ControlDeleter<T>>(std::forward<Args>(args)...);
        c->onCreated();
        return c;
    }

    Control(EngineContext &engine_context);
    virtual ~Control();

    Gui *getGui() const;

    s_ptr<Control> getParent() const;
    void setParent(const s_ptr<Control> &parent);

    const std::vector<s_ptr<Control>> &getChildren() const;

    int32_t getState() const;
    void setState(int32_t state);

    bool isEnable() const;
    void setEnable(bool enable);

    bool isVisible() const;
    void setVisible(bool visible);

    const vec4 &getPadding() const;
    void setPadding(const vec4 &padding);

    const vec2 &getImplicitSize() const;
    void setImplicitSize(const vec2 &implicit_size);

    const vec2 &getSize() const;
    void setSize(const vec2 &size);

    const vec2 &getPosition() const;
    void setPosition(const vec2 &position);

    const mat4 &getTransform() const;

    s_ptr<Control> findContolAtPos(const vec2 &pos, vec2 *control_global_position = nullptr);
    bool contains(const vec2 &pos) const;

    float getFontPixelHeight() const;
    void setFontPixelHeight(float pixel_size);

    const s_ptr<Font> &getFont() const;
    void setFont(const s_ptr<Font> &font);

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
    virtual void updateImplicitSize();

private:
    void updateGui(Gui *gui);

private:
    Gui *m_gui;

    std::vector<s_ptr<Control>> m_children;
    WeakPtr<Control> m_parent;

    int32_t m_state;

    bool m_enable;
    bool m_visible;

    // left, top, right, bottom
    vec4 m_padding;
    vec2 m_implicit_size;
    vec2 m_size;
    vec2 m_position;
    mutable mat4 m_transform;
    mutable bool m_transform_dirty;

    float m_font_pixel_height;
    s_ptr<Font> m_font;

    mutable Batch2D m_batch_2d;
    mutable bool m_draw_dirty;

    bool m_destroyed;
};

} // namespace ae::gui

#endif // AE_GUI_CONTROL_H
