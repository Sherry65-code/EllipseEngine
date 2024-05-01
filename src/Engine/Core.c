#include "Core.h"

GLFWwindow* gWindow = nullptr;
VkInstance gInstance = nullptr;

void ePassWindowPointer(GLFWwindow* window) {
	gWindow = window;
}

void eCreateInstance() {
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Eclipse Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo
	};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	// TEMP

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
	assert(extensions != nullptr);
	
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);


	for (int i = 0; i < extensionCount; i++)
		printf("%s\n", extensions[i].extensionName);

	if (vkCreateInstance(&createInfo, nullptr, &gInstance) != VK_SUCCESS)
		eThrowError("Could not create Instance!");

	free(extensions);
}

void eCoreCleanup() {
	vkDestroyInstance(gInstance, nullptr);
}