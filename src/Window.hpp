#include <iostream>

#include <GLFW/glfw3.h>

class Window {
public:
	typedef GLFWwindow* WINDOW;

	WINDOW gWindow = nullptr;
	uint8_t NewWindow(std::string title, uint32_t width, uint32_t height, bool isFullscreen, bool isResizable);
	WINDOW Return() const;
	void Cleanup();
};