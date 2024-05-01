#include "Engine.h"

EInitInfo gInitInfo;

void eInitEngine(EInitInfo initInfo) {
	gInitInfo = initInfo;
}

void eInitWindow() {

	// Initialize GLFW
	if (!glfwInit())
		eThrowError("Window System could not be Initialized!");

	// Check for Vulkan Support
	if (!glfwVulkanSupported())
		eThrowError("Vulkan compaible device not found!");

	// Set GLFW to not use OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Check if window needs to be resizable or not
	switch (gInitInfo.isResizable) {
	case true:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		break;
	case false:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		break;
	};

	// Create Window
	gInitInfo.pWindow = glfwCreateWindow(gInitInfo.width, gInitInfo.height, gInitInfo.title, nullptr, nullptr);

	// Check if window is created
	if (!gInitInfo.pWindow) {
		glfwTerminate();
		eThrowError("Could not create Window!");
	}

	// set current context to this window
	glfwMakeContextCurrent(gInitInfo.pWindow);
}

void eInitVulkan() {
	eCreateInstance();
	eSetupDebugMessenger();
}

void eMainLoop() {
	while (!glfwWindowShouldClose(gInitInfo.pWindow)) {
		glfwPollEvents();
	}
}

void eRun() {
	eInitWindow();
	ePassWindowPointer(gInitInfo.pWindow);
	eInitVulkan();
	eMainLoop();
	eCleanup();
}

void eCleanup() {
	eCoreCleanup();

	glfwDestroyWindow(gInitInfo.pWindow);
	glfwTerminate();
}