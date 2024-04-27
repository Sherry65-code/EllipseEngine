#include "Core.h"

VkInstance g_Instance = VK_NULL_HANDLE;
VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice g_Device = VK_NULL_HANDLE;
VkQueue g_GraphicsQueue = VK_NULL_HANDLE;

uint32_t g_GraphicsFamily = VK_NULL_HANDLE;

GLFWwindow* g_Window = nullptr;
VkDebugUtilsMessengerEXT g_DebugMessenger;

const char* g_ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

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

    if (g_PhysicalDevice == VK_NULL_HANDLE || max == 0)
        eThrowError("Failed to find GPU capable of running this engine!");

    free(devices);
}

void _eCreateLogicalDevice() {
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = g_GraphicsFamily,
        .queueCount = 1
    };

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {
        .multiViewport = false
    };

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queueCreateInfo,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = 0
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

    vkGetDeviceQueue(g_Device, g_GraphicsFamily, 0, &g_GraphicsQueue);
}

void _eFindQueueFamilies() {
    uint32_t graphicsFamily = UINT32_MAX;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkQueueFamilyProperties queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
            break;
        }
    }

    free(queueFamilies);

    g_GraphicsFamily = graphicsFamily;
}

uint32_t _eRateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
    score += deviceProperties.limits.maxImageDimension2D;

    printf("Score for GPU %s is %d\n", deviceProperties.deviceName, score);

    if (!deviceFeatures.geometryShader) return 0;

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
    vkDestroyDevice(g_Device, nullptr);
    
    if (g_EnableValidationLayers)
        _eDestroyDebugUtilsMessengerEXT(g_Instance, g_DebugMessenger, nullptr);

    vkDestroyInstance(g_Instance, nullptr);
}
