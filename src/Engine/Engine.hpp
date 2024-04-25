#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define E_SUCCESS true
#define E_FAILURE false

class Eclipse {
public:
	typedef GLFWwindow* EWindow;
	typedef bool EResult;
	typedef struct ECreateInfo {
		EWindow pWindow;
		uint32_t width;
		uint32_t height;
		std::string title;
		bool isFullscreen;
		bool isResizable;
	} ECreateInfo;

	EResult eInitEngine(ECreateInfo createInfo);
	EResult eRun();

	void eThrowError(std::string error);

private:
	ECreateInfo g_createInfo;

	void eMainLoop();
	void eCleanup();
};