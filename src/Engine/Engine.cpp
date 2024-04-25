#include "Engine.hpp"

Eclipse::EResult Eclipse::eInitEngine(ECreateInfo createInfo) {
	
	g_createInfo = createInfo;

	// Init GLFW
	if (!glfwInit())
		return E_FAILURE;

	if (!glfwVulkanSupported())
		return E_FAILURE;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	switch (g_createInfo.isResizable) {
	case true:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		break;
	case false:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		break;
	};

	g_createInfo.pWindow = glfwCreateWindow(g_createInfo.width, g_createInfo.height, g_createInfo.title.c_str(), nullptr, nullptr);

	if (!g_createInfo.pWindow) {
		glfwTerminate();
		return E_FAILURE;
	}

	glfwMakeContextCurrent(g_createInfo.pWindow);

	return E_SUCCESS;
}

Eclipse::EResult Eclipse::eRun() {

	// All Run Commands
	eMainLoop();
	eCleanup();

	return E_SUCCESS;
}

void Eclipse::eMainLoop() {
	while (!glfwWindowShouldClose(g_createInfo.pWindow)) {
		glfwPollEvents();
	}
}

void Eclipse::eCleanup() {
	glfwTerminate();
}

void Eclipse::eThrowError(std::string error) {
	std::cout << "ERROR: " << error << "\n";
	exit(EXIT_FAILURE);
}