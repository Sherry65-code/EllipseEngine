#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Error.h"
#include "TypeDefs.h"

typedef struct EQueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
} EQueueFamilyIndices;

void _ePassWindowPointer(GLFWwindow* window);

void _eCreateInstance();
void _eSetupDebugMessenger();
void _eCreateSurface();
void _ePickPhysicalDevice();
void _eCreateLogicalDevice();

EQueueFamilyIndices _eFindQueueFamilies(VkPhysicalDevice device);
bool _eCheckValidationLayerSupport();
const char** _eGetRequiredExtensions();
VkResult _eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void _eDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
uint32_t _eRateDeviceSuitability(VkPhysicalDevice device);
bool _eCheckDeviceExtensionSupport(VkPhysicalDevice device);

void _eCoreCleanup();

#endif