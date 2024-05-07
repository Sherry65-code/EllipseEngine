#include "VkCore.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

#if !defined(NDEBUG)
VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
	Console::Debug(callback_data->pMessage);
	return vk::False;
}
#endif

const std::vector<const char*> required_validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

Core::Instance Core::CreateInstance(InstanceCreateInfo createInfo) {
	vk::Instance instance;

	static vk::DynamicLoader dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	std::vector<vk::ExtensionProperties> available_instance_extensions = vk::enumerateInstanceExtensionProperties();

	std::vector<const char*> active_instance_extensions;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; i++)
		active_instance_extensions.push_back(glfwExtensions[i]);

#if !defined(NDEBUG)
	active_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	if (!ValidateExtensions(active_instance_extensions, available_instance_extensions))
		Console::Error("Required Extension Not Found!");

	std::vector<vk::LayerProperties> supported_validation_layers = vk::enumerateInstanceLayerProperties();
	std::vector<const char*> requested_validation_layers(required_validation_layers);

	if (!ValidateLayers(requested_validation_layers, supported_validation_layers))
		Console::Error("Required Layers Not Found!");

	vk::ApplicationInfo app(createInfo.applicationName.c_str(), {}, createInfo.engineName.c_str(), {}, createInfo.apiVersion);

	vk::InstanceCreateInfo instance_info({}, &app, requested_validation_layers, active_instance_extensions);

	vk::DebugUtilsMessengerCreateInfoEXT debug_utils_create_info;

#if !defined(NDEBUG)
	debug_utils_create_info =
		vk::DebugUtilsMessengerCreateInfoEXT({},
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			debug_utils_messenger_callback);

	instance_info.pNext = &debug_utils_create_info;
#endif

#if (defined(VKB_ENABLE_PORTABILITY))
	instance_info.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
	if (volkInitialize())
	{
		Console::Error("Failed to Initialize Vulkan Dynamic Loader!");
	}
	volkLoadInstance(instance);
#endif

	return instance;
}

void Core::EnableValidationLayers(Core::Instance instance) {
}

void Core::Cleanup(Core::Objects objects) {

}

bool Core::ValidateExtensions(std::vector<const char*> active_extensions, std::vector<vk::ExtensionProperties> available_extensions) {
	for (uint32_t i = 0; i < active_extensions.size(); i++) {
		bool isFound = false;
		for (uint32_t j = 0; j < available_extensions.size(); j++) {
			if (strcmp(active_extensions[i], available_extensions[j].extensionName) == 0)
				isFound = true;
		}
		if (!isFound)
			return false;
	}
	return true;
}

bool Core::ValidateLayers(std::vector<const char*> active_layers, std::vector<vk::LayerProperties> available_layers) {
	for (uint32_t i = 0; i < active_layers.size(); i++) {
		bool isFound = false;
		for (uint32_t j = 0; j < available_layers.size(); j++) {
			if (strcmp(active_layers[i], available_layers[j].layerName) == 0)
				isFound = true;
		}
		if (!isFound)
			return false;
	}
	return true;
}