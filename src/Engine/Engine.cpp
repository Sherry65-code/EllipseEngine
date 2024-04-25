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
	eInitVulkan();
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
	vkDestroyInstance(g_instance, nullptr);

	glfwDestroyWindow(g_createInfo.pWindow);

	glfwTerminate();
}

void Eclipse::eThrowError(std::string error) {
	std::cout << "ERROR: " << error << "\n";
	exit(EXIT_FAILURE);
}

void Eclipse::eInitVulkan() {
	eCreateInstance();
}

void Eclipse::eCreateInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = g_createInfo.title.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Eclipse Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &g_instance) != VK_SUCCESS)
		eThrowError("ENGINE: Could Not Create Instance!");
}