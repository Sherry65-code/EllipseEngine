#include "Window.hpp"

Window::WINDOW Window::createWindow(int width, int height, const char* title, bool isResizable, bool isFullscreen) {
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    if (!isResizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (isFullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        return glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
    }

    pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    
    return pWindow;
}

bool Window::shouldWindowClose() {
    return glfwWindowShouldClose(pWindow);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::getInstanceExtensions(uint32_t& extensionCount, const char**& extensions) {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensionCount = glfwExtensionCount;
    extensions = glfwExtensions;
}

void Window::destroyWindow() {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}