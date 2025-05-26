#include "glfw_manager.h"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace ae {

GLFWManager::GLFWManager()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
}

GLFWManager::~GLFWManager()
{
    glfwTerminate();
}

} // namespace ae
