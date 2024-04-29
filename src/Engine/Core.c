#include "Core.h"

VkImage* g_SwapChainImages = VK_NULL_HANDLE;

VkInstance g_Instance = VK_NULL_HANDLE;
VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice g_Device = VK_NULL_HANDLE;
VkQueue g_GraphicsQueue = VK_NULL_HANDLE;
VkQueue g_PresentQueue = VK_NULL_HANDLE;
VkSurfaceKHR g_Surface = VK_NULL_HANDLE;
VkSwapchainKHR g_SwapChain = VK_NULL_HANDLE;
VkFormat g_SwapChainImageFormat = (VkFormat)0;
VkExtent2D g_SwapChainExtent = { 0 };

GLFWwindow* g_Window = nullptr;
VkDebugUtilsMessengerEXT g_DebugMessenger;

const char* g_ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };
const char* g_DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
const bool g_EnableValidationLayers = false;
#else
const bool g_EnableValidationLayers = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL _eDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    printf("VALIDATION LAYER MESSAGE: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

void _ePassWindowPointer(GLFWwindow* window) {
    g_Window = window;
}

void _eCreateInstance() {
    if (g_EnableValidationLayers && !_eCheckValidationLayerSupport())
        eThrowError("Validation Layers requested but not found!");

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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = _eDebugCallback
    };

    const char** extensions = _eGetRequiredExtensions();
    createInfo.enabledExtensionCount = (uint32_t)(sizeof(extensions) / sizeof(extensions[0]));
    createInfo.ppEnabledExtensionNames = extensions;

    if (g_EnableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)(sizeof(g_ValidationLayers) / sizeof(g_ValidationLayers[0]));
        createInfo.ppEnabledLayerNames = g_ValidationLayers;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.enabledExtensionCount = glfwExtensionCount + 1;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &g_Instance) != VK_SUCCESS)
        eThrowError("Could not create Vulkan instance!");

    free(extensions);
}

void _eCreateSurface() {
    if (glfwCreateWindowSurface(g_Instance, g_Window, nullptr, &g_Surface) != VK_SUCCESS)
        eThrowError("Failed to create window surface!");
}

void _ePickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        eThrowError("Failed to find Vulkan Compatible GPU!");

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(deviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices);

    uint32_t max = 0;
    int index = 0;

    for (uint32_t i = 0; i < deviceCount; i++) {
        if (max < _eRateDeviceSuitability(devices[i])) {
            max = _eRateDeviceSuitability(devices[i]);
            index = i;
        }
    }

    g_PhysicalDevice = devices[index];
        
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(g_PhysicalDevice, &deviceProperties);

    printf("GPU Selected = %s\n", deviceProperties.deviceName);
    printf("GPU Score = %d\n", _eRateDeviceSuitability(g_PhysicalDevice));

    if (g_PhysicalDevice == VK_NULL_HANDLE || max == 0)
        eThrowError("Your GPU doesnt support features required by this engine!");

    free(devices);
}

void _eCreateLogicalDevice() {
    EQueueFamilyIndices indices = _eFindQueueFamilies(g_PhysicalDevice);

    VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(sizeof(VkDeviceQueueCreateInfo) * 2);
    uint32_t uniqueQueueFamilies[2] = { indices.graphicsFamily, indices.presentFamily };

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


    VkPhysicalDeviceFeatures deviceFeatures = { VK_NULL_HANDLE };

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfos,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = (sizeof(g_DeviceExtensions) / sizeof(g_DeviceExtensions[0])),
        .ppEnabledExtensionNames = g_DeviceExtensions
    };

    if (g_EnableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)(sizeof(g_ValidationLayers) / sizeof(g_ValidationLayers[0]));
        createInfo.ppEnabledLayerNames = g_ValidationLayers;
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(g_PhysicalDevice, &createInfo, nullptr, &g_Device) != VK_SUCCESS)
        eThrowError("Failed to create Logical Device!");

    vkGetDeviceQueue(g_Device, indices.graphicsFamily, 0, &g_GraphicsQueue);
    vkGetDeviceQueue(g_Device, indices.presentFamily, 0, &g_PresentQueue);

    free(queueCreateInfos);
}

void _eCreateSwapChain() {
    ESwapChainSupportDetails swapChainSupport = _eQuerySwapChainSupport(g_PhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = _eChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = _eChooseSwapPresentMode(&swapChainSupport.presentModes);
    VkExtent2D extent = _eChooseSwapExtent(swapChainSupport.capabilites);

    uint32_t imageCount = _eClamp(swapChainSupport.capabilites.minImageCount + 1, swapChainSupport.capabilites.minImageCount, swapChainSupport.capabilites.maxImageCount);
    
    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = g_Surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    };

    EQueueFamilyIndices indices = _eFindQueueFamilies(g_PhysicalDevice);
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
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(g_Device, &createInfo, nullptr, &g_SwapChain) != VK_SUCCESS)
        eThrowError("Failed to create Swap Chain!");

    vkGetSwapchainImagesKHR(g_Device, g_SwapChain, &imageCount, nullptr);
    g_SwapChainImages = (VkImage*)malloc(sizeof(VkImage) * imageCount);
    vkGetSwapchainImagesKHR(g_Device, g_SwapChain, &imageCount, g_SwapChainImages);
    
    g_SwapChainImageFormat = surfaceFormat.format;
    g_SwapChainExtent = extent;
}

ESwapChainSupportDetails _eQuerySwapChainSupport(VkPhysicalDevice device) {
    ESwapChainSupportDetails* details = (ESwapChainSupportDetails*)malloc(sizeof(ESwapChainSupportDetails));
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, g_Surface, &details->capabilites);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_Surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details->formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_Surface, &formatCount, details->formats);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_Surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details->presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_Surface, &presentModeCount, &details->presentModes);
    }

    ESwapChainSupportDetails return_value = *(details);

    free(details);

    return return_value;
}

EQueueFamilyIndices _eFindQueueFamilies(VkPhysicalDevice device) {
    EQueueFamilyIndices indices = { UINT32_MAX, UINT32_MAX };

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkQueueFamilyProperties queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g_Surface, &presentSupport);

        if (presentSupport)
            indices.presentFamily = i;
        if (indices.graphicsFamily != UINT32_MAX && indices.presentFamily != UINT32_MAX)
            break;
    }

    free(queueFamilies);

    return indices;
}

VkSurfaceFormatKHR _eChooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats) {
    for (uint32_t i = 0; i < (sizeof(availableFormats) / sizeof(availableFormats[0])); i++) {
        VkSurfaceFormatKHR availableFormat = availableFormats[i];
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }
    return availableFormats[0];
}

VkPresentModeKHR _eChooseSwapPresentMode(VkPresentModeKHR* availablePresentModes) {
    
    for (uint32_t i = 0; i < (sizeof(availablePresentModes) / sizeof(availablePresentModes[0])); i++) {
        VkPresentModeKHR availablePresentMode = availablePresentModes[i];
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D _eChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilites) {
    if (capabilites.currentExtent.width != UINT32_MAX)
        return capabilites.currentExtent;
    else {
        int width = 0, height = 0;
        glfwGetFramebufferSize(g_Window, &width, &height);

        VkExtent2D actualExtent = {
            (uint32_t)width,
            (uint32_t)height
        };

        actualExtent.width = _eClamp(actualExtent.width, capabilites.minImageExtent.width, capabilites.maxImageExtent.width);
        actualExtent.height = _eClamp(actualExtent.height, capabilites.minImageExtent.height, capabilites.maxImageExtent.height);
    
        return actualExtent;
    }
}

bool _eCheckDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    VkExtensionProperties* avaiableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avaiableExtensions);

    char** requiredExtensions = (char**)malloc(sizeof(char*) * (extensionCount));
    requiredExtensions = g_DeviceExtensions;

    for (int i = 0; i < (sizeof(requiredExtensions) / sizeof(requiredExtensions[0])); i++) {
        char* extension_to_search = requiredExtensions[i];
        bool isFound = false;
        for (int j = 0; j < extensionCount; j++) {
            if (strcmp(extension_to_search, avaiableExtensions[j].extensionName) == 0) {
                isFound = true;
                break;
            }
        }
        if (!isFound)
            return false;
    }
     
    return true;
}

uint32_t _eRateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    EQueueFamilyIndices indices = _eFindQueueFamilies(device);

    int score = 0;
    
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    
    score += deviceProperties.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader) return 0;
    if (!_eCheckDeviceExtensionSupport(device)) return 0;
    if (indices.graphicsFamily == UINT32_MAX || indices.presentFamily == UINT32_MAX) return 0;
    return score;
}

bool _eCheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (size_t i = 0; i < sizeof(g_ValidationLayers) / sizeof(g_ValidationLayers[0]); i++) {
        bool layerFound = false;

        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(g_ValidationLayers[i], availableLayers[j].layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            free(availableLayers);
            return false;
        }
    }

    free(availableLayers);
    return true;
}

const char** _eGetRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char** extensions = (const char**)malloc((sizeof(const char**) * (glfwExtensionCount + 1)));

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions[i] = glfwExtensions[i];
    }

    if (g_EnableValidationLayers)
        extensions[glfwExtensionCount] = "VK_EXT_debug_utils";

    return extensions;
}

void _eSetupDebugMessenger() {
    if (!g_EnableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = _eDebugCallback
    };

    if (_eCreateDebugUtilsMessengerEXT(g_Instance, &createInfo, nullptr, &g_DebugMessenger) != VK_SUCCESS)
        eThrowError("Failed to setup Debug Messenger!");
}

VkResult _eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void _eDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void _eCoreCleanup() {
    vkDestroySwapchainKHR(g_Device, g_SwapChain, nullptr);
    vkDestroyDevice(g_Device, nullptr);
    
    if (g_EnableValidationLayers)
        _eDestroyDebugUtilsMessengerEXT(g_Instance, g_DebugMessenger, nullptr);

    vkDestroySurfaceKHR(g_Instance, g_Surface, nullptr);
    vkDestroyInstance(g_Instance, nullptr);

    free(g_SwapChainImages);
}