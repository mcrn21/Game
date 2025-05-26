#include "window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ae {

void GLFWWindowDeleter::operator()(GLFWwindow *window)
{
    if (window)
        glfwDestroyWindow(window);
}

Window::Window()
    : m_mouse_enabled{true}
{}

Window::Window(int32_t width, int32_t height, const std::string &title)
    : m_mouse_enabled{true}
{
    create(width, height, title);
}

bool Window::create(int32_t width, int32_t height, const std::string &title)
{
    if (m_window)
        return false;

    if (width == 0 || height == 0)
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_window = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(
        glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));

    if (!m_window)
        return false;

    glfwSetWindowUserPointer(m_window.get(), this);

    glfwSetMouseButtonCallback(m_window.get(), &Window::mouseButtonCallback);
    glfwSetCursorPosCallback(m_window.get(), &Window::cursorPosCallback);
    glfwSetKeyCallback(m_window.get(), &Window::keyCallback);
    glfwSetScrollCallback(m_window.get(), &Window::scrollCallback);
    glfwSetWindowSizeCallback(m_window.get(), &Window::sizeCallback);

    glfwMakeContextCurrent(m_window.get());

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        destroy();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setViewport({0, 0, width, height});
    setSize({width, height});

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
    return glfwWindowShouldClose(m_window.get());
}

void Window::setShouldClose(bool flag)
{
    if (m_window)
        glfwSetWindowShouldClose(m_window.get(), flag);
}

bool Window::isMouseEnabled() const
{
    if (!m_window)
        return false;
    return m_mouse_enabled;
}

void Window::setMouseEnabled(bool enable)
{
    if (m_window) {
        m_mouse_enabled = enable;
        glfwSetInputMode(m_window.get(),
                         GLFW_CURSOR,
                         m_mouse_enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

void Window::pollEvents()
{
    m_input.update();
    glfwPollEvents();
}

void Window::display() const
{
    if (m_window)
        glfwSwapBuffers(m_window.get());
}

void Window::keyCallback(
    GLFWwindow *glfw_window, int32_t keycode, int32_t scancode, int32_t action, int32_t mode)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    window->getInput().setKeyPressed(static_cast<KeyCode>(keycode), pressed);
}

void Window::mouseButtonCallback(GLFWwindow *glfw_window,
                                 int32_t button,
                                 int32_t action,
                                 int32_t mode)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    window->getInput().setButtonPressed(static_cast<ButtonCode>(button), pressed);
}

void Window::cursorPosCallback(GLFWwindow *glfw_window, double x, double y)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->getInput().setCursorPosition(static_cast<int32_t>(x), static_cast<int32_t>(y));
}

void Window::scrollCallback(GLFWwindow *glfw_window, double xoffset, double yoffset)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->getInput().setScrollX(xoffset);
    window->getInput().setScrollY(yoffset);
}

void Window::sizeCallback(GLFWwindow *glfw_window, int32_t width, int32_t height)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    window->setViewport({0, 0, width, height});
    window->setSize({width, height});
}

} // namespace ae
