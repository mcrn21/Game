#ifndef AE_WINDOW_H
#define AE_WINDOW_H

#include "../graphics/core/render_target.h"
#include "input.h"

#include <memory>

struct GLFWwindow;

namespace ae {

struct GLFWWindowDeleter
{
    void operator()(GLFWwindow *window);
};

class Window : public RenderTarget
{
public:
    Window();
    Window(int32_t width, int32_t height, const std::string &title);
    ~Window() = default;

    bool create(int32_t width, int32_t height, const std::string &title);
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
    static void keyCallback(
        GLFWwindow *glfw_window, int32_t keycode, int32_t scancode, int32_t action, int32_t mode);
    static void mouseButtonCallback(GLFWwindow *glfw_window,
                                    int32_t button,
                                    int32_t action,
                                    int32_t mode);
    static void cursorPosCallback(GLFWwindow *glfw_window, double x, double y);
    static void scrollCallback(GLFWwindow *glfw_window, double xoffset, double yoffset);
    static void sizeCallback(GLFWwindow *glfw_window, int32_t width, int32_t height);

private:
    std::unique_ptr<GLFWwindow, GLFWWindowDeleter> m_window;
    Input m_input;
    bool m_mouse_enabled;
};

} // namespace ae

#endif // AE_WINDOW_H
