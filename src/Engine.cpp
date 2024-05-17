#include "Engine.hpp"

bool isDebug = false;

Core core;

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
    pWindow = Window::createWindow(800, 600, "Eclipse Engine Demo", false, false);
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
    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        core.drawFrame();
    }

    core.deviceWaitIdle();
}

void Engine::cleanup() {
    core.cleanup();   
    Window::destroyWindow(pWindow);
}