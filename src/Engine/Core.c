#include "Core.h"

#ifdef NDEBUG
const bool gEnableValidationLayers = false;
#else
const bool gEnableValidationLayers = true;
#endif

GLFWwindow* gWindow = nullptr;
VkInstance gInstance = nullptr;
VkDebugUtilsMessengerEXT gDebugMessenger = nullptr;
VkPhysicalDevice gPhysicalDevice = nullptr;
VkDevice gDevice = nullptr;
VkQueue gGraphicsQueue = nullptr;
VkQueue gPresentQueue = nullptr;
VkSurfaceKHR gSurface = nullptr;
VkSwapchainKHR gSwapChain = nullptr;
VkFormat gSwapChainImageFormat;
VkExtent2D gSwapChainExtent = { nullptr };

VkImage* gSwapChainImages = nullptr;
uint32_t gSwapChainImagesCount = 0;

VkImageView* gSwapChainImageViews = nullptr;
uint32_t gSwapChainImageViewsCount = 0;

const char* gValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

const char* gDeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

static VKAPI_ATTR VkBool32 VKAPI_CALL eDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	printf("DEBUG: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

static bool eHasValue(EQueueFamilyIndices indices) {
	return (indices.graphicsFamily != UINT32_MAX) && (indices.presentFamily != UINT32_MAX);
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

void eCreateSurface() {
	if (glfwCreateWindowSurface(gInstance, gWindow, nullptr, &gSurface) != VK_SUCCESS)
		eThrowError("Failed to create Window Surface!");
}

void ePickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(gInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
		eThrowError("Failed to find a GPU!");

	VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
	assert(physicalDevices != nullptr);
	vkEnumeratePhysicalDevices(gInstance, &deviceCount, physicalDevices);

	uint32_t max = 0;
	uint8_t index = 0;

	for (uint8_t i = 0; i < deviceCount; i++) {
		VkPhysicalDevice _device = physicalDevices[i];
		uint32_t score = eRateDevice(_device);
		
		if (score > max) {
			max = score;
			index = i;
		}
	}

	if (max == 0)
		eThrowError("Failed to find a GPU capable to run this engine!");

	gPhysicalDevice = physicalDevices[index];

	free(physicalDevices);
}

void eCreateLogicalDevice() {
	EQueueFamilyIndices indices = eFindQueueFamilies(gPhysicalDevice);

	VkPhysicalDeviceFeatures deviceFeatures = { nullptr };

	VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(sizeof(VkDeviceQueueCreateInfo) * 2);
	assert(queueCreateInfos != nullptr);
	uint32_t uniqueQueueFamilies[] = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (uint32_t i = 0; i < 2; i++) {
		uint32_t queueFamily = uniqueQueueFamilies[i];
		
		VkDeviceQueueCreateInfo queueCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};

		queueCreateInfos[i] = queueCreateInfo;
	}

	VkDeviceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = queueCreateInfos,
		.queueCreateInfoCount = 2,
		.pEnabledFeatures = &deviceFeatures,
		.enabledExtensionCount = sizeof(gDeviceExtensions) / sizeof(gDeviceExtensions[0]),
		.ppEnabledExtensionNames = gDeviceExtensions
	};

	if (gEnableValidationLayers) {
		createInfo.enabledLayerCount = sizeof(gValidationLayers) / sizeof(gValidationLayers[0]);
		createInfo.ppEnabledLayerNames = gValidationLayers;
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(gPhysicalDevice, &createInfo, nullptr, &gDevice) != VK_SUCCESS)
		eThrowError("Failed to create Logical Device!");
	
	vkGetDeviceQueue(gDevice, indices.graphicsFamily, 0, &gGraphicsQueue);
	vkGetDeviceQueue(gDevice, indices.presentFamily, 0, &gPresentQueue);

	free(queueCreateInfos);
}

void eCreateSwapChain() {
	ESwapChainSupportDetails swapChainSupport = eQuerySwapChainSupport(gPhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = eChooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatsCount);
	VkPresentModeKHR presentMode = eChooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModesCount);
	VkExtent2D extent = eChooseSwapExtent(swapChainSupport.capabilites);

	uint32_t imageCount = swapChainSupport.capabilites.minImageCount + 1;

	if (swapChainSupport.capabilites.maxImageCount > 0 && imageCount > swapChainSupport.capabilites.maxImageCount)
		imageCount = swapChainSupport.capabilites.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = gSurface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
	};

	EQueueFamilyIndices indices = eFindQueueFamilies(gPhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;	
	}

	createInfo.preTransform = swapChainSupport.capabilites.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = nullptr;

	if (vkCreateSwapchainKHR(gDevice, &createInfo, nullptr, &gSwapChain) != VK_SUCCESS)
		eThrowError("Failed to create swap chain!");

	vkGetSwapchainImagesKHR(gDevice, gSwapChain, &imageCount, nullptr);
	gSwapChainImages = (VkImage*)malloc(sizeof(VkImage) * imageCount);
	assert(gSwapChainImages != nullptr);
	gSwapChainImagesCount = imageCount;
	vkGetSwapchainImagesKHR(gDevice, gSwapChain, &imageCount, gSwapChainImages);

	gSwapChainImageFormat = surfaceFormat.format;
	gSwapChainExtent = extent;
}

void eCreateImageViews() {
	gSwapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * gSwapChainImagesCount);
	assert(gSwapChainImageViews != nullptr);
	gSwapChainImageViewsCount = gSwapChainImagesCount;

	for (uint32_t i = 0; i < gSwapChainImagesCount; i++) {
		VkImageViewCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = gSwapChainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = gSwapChainImageFormat,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1
		};

		if (vkCreateImageView(gDevice, &createInfo, nullptr, &gSwapChainImageViews[i]) != VK_SUCCESS)
			eThrowError("Failed to create Image Views!");
	}

}

void eCreateGraphicsPipeline() {

}

uint32_t eRateDevice(VkPhysicalDevice device) {
	EQueueFamilyIndices indices = eFindQueueFamilies(device);

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	uint32_t score = 0;
	bool swapChainAdequate = false;

	score += deviceProperties.limits.maxImageDimension2D;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
	if (!deviceFeatures.geometryShader) return 0;
	if (!eHasValue(indices)) return 0;
	if (!eCheckDeviceExtensionSupport(device)) return 0;
	else {
		ESwapChainSupportDetails swapChainSupport = eQuerySwapChainSupport(device);
		swapChainAdequate = !(swapChainSupport.formats == nullptr) && !(swapChainSupport.presentModes == nullptr);
		if (swapChainAdequate)
			score += 1000;
		else
			return 0;
	}

	return score;

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

EQueueFamilyIndices eFindQueueFamilies(VkPhysicalDevice device) {
	EQueueFamilyIndices indices = { UINT32_MAX, UINT32_MAX };

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	assert(queueFamilies != nullptr);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VkQueueFamilyProperties queueFamily = queueFamilies[i];
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, gSurface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;
	}

	free(queueFamilies);

	return indices;
}

bool eCheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	VkExtensionProperties* availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
	assert(availableExtensions != nullptr);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions);

	uint32_t requiredExtensionsCount = sizeof(gDeviceExtensions) / sizeof(gDeviceExtensions[0]);

	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		bool isFound = false;
		for (uint32_t j = 0; j < extensionCount; j++) {
			if (strcmp(gDeviceExtensions[i], availableExtensions[j].extensionName) == 0) {
				isFound = true;
				break;
			}
		}
		if (!isFound) {
			free(availableExtensions);
			return false;
		}
	}

	free(availableExtensions);
	return true;
}

ESwapChainSupportDetails eQuerySwapChainSupport(VkPhysicalDevice device) {
	ESwapChainSupportDetails details = {
		.capabilites = nullptr,
		.formats = nullptr,
		.presentModes = nullptr,
		.presentModesCount = 0,
		.formatsCount = 0
	};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, gSurface, &details.capabilites);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, gSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
		assert(details.formats != nullptr);
		details.formatsCount = formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, gSurface, &formatCount, details.formats);
	}
	else {
		eThrowError("Failed to find a Surface Format!");
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, gSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
		assert(details.presentModes != nullptr);
		details.presentModesCount = presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, gSurface, &presentModeCount, details.presentModes);
	}
	else {
		eThrowError("Failed to find a Presentation Mode!");
	}

	return details;
}

VkSurfaceFormatKHR eChooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats, uint32_t availableFormatsCount) {
	assert(availableFormats != nullptr);

	for (uint32_t i = 0; i < availableFormatsCount; i++) {
		VkSurfaceFormatKHR availableFormat = availableFormats[i];
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR eChooseSwapPresentMode(VkPresentModeKHR* availablePresentModes, uint32_t availablePresentModesCount) {
	for (uint32_t i = 0; i < availablePresentModesCount; i++) {
		VkPresentModeKHR availablePresentMode = availablePresentModes[i];

		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D eChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilites) {
	if (capabilites.currentExtent.width != UINT32_MAX)
		return capabilites.currentExtent;
	
	int width = 0, height = 0;
	glfwGetFramebufferSize(gWindow, &width, &height);

	VkExtent2D actualExtent = { width, height };

	actualExtent.width = Maths_Clamp(actualExtent.width, capabilites.minImageExtent.width, capabilites.maxImageExtent.width);
	actualExtent.height = Maths_Clamp(actualExtent.height, capabilites.minImageExtent.height, capabilites.maxImageExtent.height);
	
	return actualExtent;
}

void eCoreCleanup() {

	for (uint32_t i = 0; i < gSwapChainImageViewsCount; i++)
		vkDestroyImageView(gDevice, gSwapChainImageViews[i], nullptr);

	vkDestroySwapchainKHR(gDevice, gSwapChain, nullptr);
	vkDestroyDevice(gDevice, nullptr);

	if (gEnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(gInstance, gDebugMessenger, nullptr);

	vkDestroySurfaceKHR(gInstance, gSurface, nullptr);
	vkDestroyInstance(gInstance, nullptr);

	free(gSwapChainImages);
}