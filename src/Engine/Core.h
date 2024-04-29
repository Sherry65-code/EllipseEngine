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
#include "Maths.h"

typedef struct EQueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
} EQueueFamilyIndices;

typedef struct ESwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilites;
	VkSurfaceFormatKHR* formats;
	VkPresentModeKHR* presentModes;
} ESwapChainSupportDetails;

void _ePassWindowPointer(GLFWwindow* window);

void _eCreateInstance();
void _eSetupDebugMessenger();
void _eCreateSurface();
void _ePickPhysicalDevice();
void _eCreateLogicalDevice();
void _eCreateSwapChain();

EQueueFamilyIndices _eFindQueueFamilies(VkPhysicalDevice device);
bool _eCheckValidationLayerSupport();
const char** _eGetRequiredExtensions();
VkResult _eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void _eDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
uint32_t _eRateDeviceSuitability(VkPhysicalDevice device);
bool _eCheckDeviceExtensionSupport(VkPhysicalDevice device);
ESwapChainSupportDetails _eQuerySwapChainSupport(VkPhysicalDevice device);
VkSurfaceFormatKHR _eChooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats);
VkPresentModeKHR _eChooseSwapPresentMode(VkPresentModeKHR* availablePresentModes);
VkExtent2D _eChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilites);

void _eCoreCleanup(); 

#endif