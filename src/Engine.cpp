#include "Engine.hpp"

bool isDebug = false;

Core core;
Window window;

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    core.framebufferResized = true;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_F1:
            Console::PrintInfo("Switching to Wireframe Mode!");
            core.setRasterizerMode(Core::WIREFRAME);
            break;
        case GLFW_KEY_F2:
            Console::PrintInfo("Switching to Rasterized Mode!");
            core.setRasterizerMode(Core::FILL);
            break;
        case GLFW_KEY_F3:
            Console::PrintInfo("Switching to Point Mode!");
            core.setRasterizerMode(Core::POINT);
            break;
        };
    }
}

void Engine::setDebugMode(bool debugMode) {
    isDebug = debugMode;
}

void Engine::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Engine::initWindow() {
    pWindow = window.createWindow(800, 600, "Eclipse Engine Demo", true, false);
    glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);
    glfwSetKeyCallback(pWindow, keyCallback);
}

void Engine::initVulkan() const {
    core.setDebugMode(isDebug);
    core.setWindowPointer(pWindow);
    core.createInstance();
    core.setupDebugMessenger();
    core.createSurface();
    core.pickPhysicalDevice();
    core.createLogicalDevice();
    core.createSwapChain();
    core.createImageViews();
    core.createRenderPass();
    core.createDescriptorSetLayout();
    core.createGraphicsPipeline();
    core.createFramebuffers();
    core.createCommandPool();
    core.createVertexBuffer();
    core.createIndexBuffer();
    core.createUniformBuffers();
    core.createDescriptorPool();
    core.createDescriptorSets();
    core.createCommandBuffer();
    core.createSyncObjects();
}

void Engine::mainLoop() {
    while (!window.shouldWindowClose()) {
        window.pollEvents();
        core.drawFrame();
    }

    core.deviceWaitIdle();
}

void Engine::cleanup() {
    core.cleanup();   
    window.destroyWindow();
}