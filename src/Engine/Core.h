#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

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
	uint32_t formatsCount;
	uint32_t presentModesCount;
} ESwapChainSupportDetails;

void ePassWindowPointer(GLFWwindow* window);

void eCreateInstance();
void eSetupDebugMessenger();
void eCreateSurface();
void ePickPhysicalDevice();
void eCreateLogicalDevice();
void eCreateSwapChain();

bool eCheckValidationLayerSupport();
const char** eGetRequiredExtensions();
uint32_t eGetRequiredExtensionCount();
VkResult eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
uint32_t eRateDevice(VkPhysicalDevice device);
EQueueFamilyIndices eFindQueueFamilies(VkPhysicalDevice device);
bool eCheckDeviceExtensionSupport(VkPhysicalDevice device);
ESwapChainSupportDetails eQuerySwapChainSupport(VkPhysicalDevice device);
VkSurfaceFormatKHR eChooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats, uint32_t availableFormatsCount);
VkPresentModeKHR eChooseSwapPresentMode(VkPresentModeKHR* availablePresentModes, uint32_t availablePresentModesCount);
VkExtent2D eChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilites);

void eCoreCleanup(); 

#endif