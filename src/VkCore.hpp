#pragma once
#include <iostream>
#include <vector>
#include <string>

#if !defined(NDEBUG)
#define VK_NO_PROTOTYPES
#endif
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#if !defined(NDEBUG)
#include <Volk/volk.h>
#endif
#include <GLFW/glfw3.h>

#include "Console.hpp"

class Core {
public:
	typedef struct InstanceCreateInfo {
		std::string applicationName;
		std::string engineName;
		uint32_t apiVersion;
	} InstanceCreateInfo;

	typedef VkInstance Instance;

	typedef struct Objects {
		Instance instance;
	} Objects;

	static Instance CreateInstance(Core::InstanceCreateInfo createInfo);
	static void EnableValidationLayers(Core::Instance instance);

	static void Cleanup(Objects objects);

private:
	static bool ValidateExtensions(std::vector<const char*> active_extensions, std::vector<vk::ExtensionProperties> available_extensions);
	static bool ValidateLayers(std::vector<const char*> active_layers, std::vector<vk::LayerProperties> available_layers);
};