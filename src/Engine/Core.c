#include "Core.h"

#ifdef NDEBUG
const bool gEnableValidationLayers = false;
#else
const bool gEnableValidationLayers = true;
#endif

GLFWwindow* gWindow = nullptr;
VkInstance gInstance = nullptr;
VkDebugUtilsMessengerEXT gDebugMessenger = nullptr;

const char* gValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

static VKAPI_ATTR VkBool32 VKAPI_CALL eDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	printf("DEBUG: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

void ePassWindowPointer(GLFWwindow* window) {
	gWindow = window;
}

void eCreateInstance() {
	if (gEnableValidationLayers && !eCheckValidationLayerSupport())
		eThrowError("Validation layers requested, but not avaliable!");

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

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = eDebugCallback,
		.pUserData = nullptr
	};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	if (gEnableValidationLayers) {
		createInfo.enabledLayerCount = sizeof(gValidationLayers) / sizeof(gValidationLayers[0]);
		createInfo.ppEnabledLayerNames = gValidationLayers;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	const char** extensions = eGetRequiredExtensions();
	createInfo.enabledExtensionCount = eGetRequiredExtensionCount();
	createInfo.ppEnabledExtensionNames = extensions;

	if (vkCreateInstance(&createInfo, nullptr, &gInstance) != VK_SUCCESS)
		eThrowError("Could not create Instance!");

	free(extensions);
}

void eSetupDebugMessenger() {
	if (!gEnableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = eDebugCallback,
		.pUserData = nullptr
	};

	if (eCreateDebugUtilsMessengerEXT(gInstance, &createInfo, nullptr, &gDebugMessenger) != VK_SUCCESS)
		eThrowError("Failed to setup debug messenger!");
}

VkResult eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

bool eCheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
	assert(availableLayers != nullptr);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

	for (int i = 0; i < sizeof(gValidationLayers)/sizeof(gValidationLayers[0]); i++) {
		const char* layerName = gValidationLayers[i];
		bool layerFound = false;

		for (int j = 0; j < layerCount; j++) {
			VkLayerProperties layerProperties = availableLayers[j];
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	free(availableLayers);

	return true;
}

const char** eGetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = nullptr;

	glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	const char** extensions;

	if (gEnableValidationLayers)
		extensions = (const char**)malloc((sizeof(const char*) * (glfwExtensionCount + 1)));
	else
		extensions = (const char**)malloc(sizeof(const char*) * glfwExtensionCount);
	assert(extensions != nullptr);
	
	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		extensions[i] = strdup(glfwExtensions[i]);
	}

	if (gEnableValidationLayers)
		extensions[glfwExtensionCount] = strdup(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

uint32_t eGetRequiredExtensionCount() {
	uint32_t glfwExtensionCount = 0;
	glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	if (gEnableValidationLayers) return glfwExtensionCount + 1;
	else return glfwExtensionCount;
}


void eCoreCleanup() {
	if (gEnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(gInstance, gDebugMessenger, nullptr);

	vkDestroyInstance(gInstance, nullptr);
}