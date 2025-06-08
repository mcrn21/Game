#include "window.h"
#include "../system/log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ae {

namespace priv {

class GLFWManager
{
public:
    GLFWManager()
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
    }

    ~GLFWManager() { glfwTerminate(); }
};

struct GLFWWindowDeleter
{
    void operator()(GLFWwindow *glfw_window)
    {
        if (glfw_window)
            glfwDestroyWindow(glfw_window);
    }
};

struct GLFWWindowImpl : public WindowImpl
{
    bool create(Window *window, const ivec2 &size, const std::string &title, int32_t msaa = 0)
    {
        if (size.x == 0 || size.y == 0)
            return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, msaa);

        glfw_window = u_ptr<GLFWwindow, GLFWWindowDeleter>(
            glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr));

        if (!glfw_window)
            return false;

        glfwSetWindowUserPointer(glfw_window.get(), window);

        glfwSetMouseButtonCallback(glfw_window.get(), &GLFWWindowImpl::mouseButtonCallback);
        glfwSetCursorPosCallback(glfw_window.get(), &GLFWWindowImpl::cursorPosCallback);
        glfwSetKeyCallback(glfw_window.get(), &GLFWWindowImpl::keyCallback);
        glfwSetScrollCallback(glfw_window.get(), &GLFWWindowImpl::scrollCallback);
        glfwSetWindowSizeCallback(glfw_window.get(), &GLFWWindowImpl::sizeCallback);
        glfwSetCharCallback(glfw_window.get(), &GLFWWindowImpl::setCharCallback);

        glfwMakeContextCurrent(glfw_window.get());

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            glfw_window.reset();
            return false;
        }

        glEnable(GL_MULTISAMPLE);

        int32_t samples = 0;
        glGetIntegerv(GL_SAMPLES, &samples);
        l_info("MSAA: {}", samples);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    bool isShouldClose() const
    {
        if (!glfw_window)
            return false;
        return glfwWindowShouldClose(glfw_window.get());
    }

    void setShouldClose(bool flag)
    {
        if (glfw_window)
            glfwSetWindowShouldClose(glfw_window.get(), flag);
    }

    bool isMouseEnabled() const { return mouse_enabled; }

    void setMouseEnabled(bool enable)
    {
        if (glfw_window) {
            mouse_enabled = enable;
            glfwSetInputMode(glfw_window.get(),
                             GLFW_CURSOR,
                             mouse_enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
    }

    void pollEvents()
    {
        if (glfw_window)
            glfwPollEvents();
    }

    void display() const
    {
        if (glfw_window)
            glfwSwapBuffers(glfw_window.get());
    }

    static void keyCallback(
        GLFWwindow *glfw_window, int32_t keycode, int32_t scancode, int32_t action, int32_t mode)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

        KeyModifier modifiers = KeyModifier::NONE;
        if (mode & GLFW_MOD_SHIFT)
            modifiers = modifiers | KeyModifier::SHIFT;
        if (mode & GLFW_MOD_CONTROL)
            modifiers = modifiers | KeyModifier::CTRL;
        if (mode & GLFW_MOD_ALT)
            modifiers = modifiers | KeyModifier::ALT;
        if (mode & GLFW_MOD_SUPER)
            modifiers = modifiers | KeyModifier::SUPER;

        window->getInput().setModifiers(modifiers);
        window->getInput().setKeyPressed(static_cast<KeyCode>(keycode), pressed);
    }

    static void mouseButtonCallback(GLFWwindow *glfw_window,
                                    int32_t button,
                                    int32_t action,
                                    int32_t mode)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        window->getInput().setButtonPressed(static_cast<ButtonCode>(button), pressed);
    }

    static void cursorPosCallback(GLFWwindow *glfw_window, double x, double y)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        window->getInput().setCursorPosition(
            ivec2{static_cast<int32_t>(x), static_cast<int32_t>(y)});
    }

    static void scrollCallback(GLFWwindow *glfw_window, double x_offset, double y_offset)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        window->getInput().setScroll(
            vec2{static_cast<float>(x_offset), static_cast<float>(y_offset)});
    }

    static void sizeCallback(GLFWwindow *glfw_window, int32_t width, int32_t height)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        window->setViewport({0, 0, width, height});
        window->setSize({width, height});
    }

    static void setCharCallback(GLFWwindow *glfw_window, uint32_t codepoint)
    {
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        window->getInput().setCodepoint(codepoint);
    }

    static GLFWManager glfw_manager;

    u_ptr<GLFWwindow, GLFWWindowDeleter> glfw_window;
    bool mouse_enabled = true;
};

GLFWManager GLFWWindowImpl::glfw_manager;

} // namespace priv

Window::Window() {}

Window::Window(const ivec2 &size, const std::string &title, int32_t msaa)
{
    create(size, title, msaa);
}

bool Window::create(const ivec2 &size, const std::string &title, int32_t msaa)
{
    destroy();
    auto window = createUnique<priv::GLFWWindowImpl>();
    if (!window->create(this, size, title, msaa))
        return false;
    m_window = std::move(window);
    setSize(size);
    setViewport({1, 1, size.x, size.y});
    return true;
}

bool Window::isValid() const
{
    return m_window.get() != nullptr;
}

void Window::destroy()
{
    if (m_window) {
        m_window.reset(nullptr);
        setSize({1, 1});
        setViewport({1, 1, 1, 1});
    }
}

Input &Window::getInput()
{
    return m_input;
}

bool Window::isShouldClose() const
{
    if (!m_window)
        return false;
    return m_window->isShouldClose();
}

void Window::setShouldClose(bool flag)
{
    if (m_window)
        m_window->setShouldClose(flag);
}

bool Window::isMouseEnabled() const
{
    if (!m_window)
        return false;
    return m_window->isMouseEnabled();
}

void Window::setMouseEnabled(bool enable)
{
    if (m_window)
        m_window->setMouseEnabled(enable);
}

void Window::pollEvents()
{
    m_input.update();
    if (m_window)
        m_window->pollEvents();
}

void Window::display() const
{
    if (m_window)
        m_window->display();
}

} // namespace ae
