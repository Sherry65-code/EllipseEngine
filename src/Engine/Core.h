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

void _ePassWindowPointer(GLFWwindow* window);

void _eCreateInstance();
void _ePickPhysicalDevice();

bool _eIsDeviceSuitable(VkPhysicalDevice device);
uint32_t _eFindQueueFamilies(VkPhysicalDevice device);
bool _eCheckValidationLayerSupport();
const char** _eGetRequiredExtensions();
void _eSetupDebugMessenger();
VkResult _eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void _eDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
uint32_t _eRateDeviceSuitability(VkPhysicalDevice device);

void _eCoreCleanup();

#endif