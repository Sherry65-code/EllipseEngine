#include "Engine.h"

EInitInfo g_InitInfo;

void eInitEngine(EInitInfo initInfo) {
	g_InitInfo = initInfo;
}

void eThrowError(char* error) {
	printf("ERROR: %s\n", error);
	exit(EXIT_FAILURE);
}

void _eInitWindow() {

	// Initialize GLFW
	if (!glfwInit())
		eThrowError("Window System could not be Initialized!");

	// Check for Vulkan Support
	if (!glfwVulkanSupported())
		eThrowError("Vulkan compaible device not found!");

	// Set GLFW to not use OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Check if window needs to be resizable or not
	switch (g_InitInfo.isResizable) {
	case true:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		break;
	case false:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		break;
	};

	// Create Window
	g_InitInfo.pWindow = glfwCreateWindow(g_InitInfo.width, g_InitInfo.height, g_InitInfo.title, nullptr, nullptr);

	// Check if window is created
	if (!g_InitInfo.pWindow) {
		glfwTerminate();
		eThrowError("Could not create Window!");
	}

	// set current context to this window
	glfwMakeContextCurrent(g_InitInfo.pWindow);
}

void _eInitVulkan() {
	
}

void _eMainLoop() {
	while (!glfwWindowShouldClose(g_InitInfo.pWindow)) {
		glfwPollEvents();
	}
}

void eRun() {
	_eInitWindow();
	_eInitVulkan();
	_eMainLoop();
}