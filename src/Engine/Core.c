#include "Core.h"

VkInstance g_Instance = VK_NULL_HANDLE;
GLFWwindow* g_Window = nullptr;

void _ePassWindowPointer(GLFWwindow* window) {
	g_Window = window;
}

void _eCreateInstance() {
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Eclipse Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions,
		.enabledLayerCount = 0
	};

	if (vkCreateInstance(&createInfo, nullptr, &g_Instance) != VK_SUCCESS)
		eThrowError("Could Not Create Vulkan Instance!");
		
}

void _eCoreCleanup() {
	vkDestroyInstance(g_Instance, nullptr);
}