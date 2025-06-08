#ifndef AE_WINDOW_H
#define AE_WINDOW_H

#include "../graphics/core/render_target.h"
#include "../system/memory.h"
#include "input.h"

#include <glm/glm.hpp>

using namespace glm;

namespace ae {

class Window;

namespace priv {

struct WindowImpl
{
    virtual ~WindowImpl() = default;

    virtual bool create(Window *window,
                        const ivec2 &size,
                        const std::string &title,
                        int32_t msaa = 0)
        = 0;

    virtual bool isShouldClose() const = 0;
    virtual void setShouldClose(bool flag) = 0;

    virtual bool isMouseEnabled() const = 0;
    virtual void setMouseEnabled(bool enable) = 0;

    virtual void pollEvents() = 0;
    virtual void display() const = 0;
};

} // namespace priv

class Window : public RenderTarget
{
public:
    Window();
    Window(const ivec2 &size, const std::string &title, int32_t msaa = 0);
    ~Window() = default;

    bool create(const ivec2 &size, const std::string &title, int32_t msaa = 0);
    bool isValid() const;
    void destroy();

    Input &getInput();

    bool isShouldClose() const;
    void setShouldClose(bool flag);

    bool isMouseEnabled() const;
    void setMouseEnabled(bool enable);

    void pollEvents();
    void display() const;

private:
    u_ptr<priv::WindowImpl> m_window;
    Input m_input;
};

} // namespace ae

#endif // AE_WINDOW_H
