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
bool _eCheckValidationLayerSupport();
const char** _eGetRequiredExtensions();
void _eSetupDebugMessenger();
VkResult _eCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void _eDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
void _eCoreCleanup();

#endif