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

void Engine::initVulkan() {
    core.setDebugMode(isDebug);
    core.createInstance();
    core.setupDebugMessenger();
    core.createSurface(pWindow);
    core.pickPhysicalDevice();
    core.createLogicalDevice();
    
}

void Engine::mainLoop() {
    Window::mainLoop(pWindow);
}

void Engine::cleanup() {
    core.cleanup();   
    Window::destroyWindow(pWindow);
}