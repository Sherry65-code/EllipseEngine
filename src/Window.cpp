#include "Window.hpp"

uint8_t Window::NewWindow(std::string title, uint32_t width, uint32_t height, bool isFullscreen, bool isResizable) {
	if (!glfwInit())
		return 1;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	if (isResizable)
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	else
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (isFullscreen) {
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		gWindow = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), primaryMonitor, nullptr);
	}
	else {
		gWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	}

	if (!gWindow)
		return 2;

	return 0;
}

Window::WINDOW Window::Return() const {
	return gWindow;
}

void Window::Cleanup() {
	if (gWindow != nullptr)
		glfwDestroyWindow(gWindow);
	glfwTerminate();
}