#include "Engine.hpp"

bool isDebug = false;

Core core;
Window window;

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    core.framebufferResized = true;
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
    core.createGraphicsPipeline();
    core.createFramebuffers();
    core.createCommandPool();
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