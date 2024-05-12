#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>

#include <vulkan/vulkan.h>

#include "Console.hpp"
#include "Window.hpp"

class Core {
    private:
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    bool isDebug = false;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    uint32_t ratePhysicalDevice(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    public:
    void setDebugMode(bool debugMode);

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();

    void cleanup();
};