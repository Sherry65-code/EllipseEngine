#include "Engine.hpp"

const std::string window_title = "Eclipse Engine Demo";
const uint32_t window_width = 800;
const uint32_t window_height = 600;
const bool window_is_fullscreen = false;
const bool window_is_resizable = false;

Window window;

Window::WINDOW gWindow = nullptr;

Core::Objects objects;

void Engine::InitWindow() {
	
	uint8_t window_return_value = window.NewWindow(window_title, window_width, window_height, window_is_fullscreen, window_is_resizable);
	
	switch (window_return_value) {
		case 0: Console::Info("Created Window Successfully!"); break;
		case 1: Console::Error("Failed to initialize window!"); break;
		case 2: Console::Error("Failed to create window!"); break;
	}

	gWindow = window.gWindow;

}

void Engine::InitRenderer() {

	Core::InstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.applicationName = window_title;
	instanceCreateInfo.engineName = "Eclipse Engine";
	instanceCreateInfo.apiVersion = VK_API_VERSION_1_1;

	// Create Instance
	objects.instance = Core::CreateInstance(instanceCreateInfo);



}

void Engine::MainLoop() {
	while (!glfwWindowShouldClose(gWindow))
		glfwPollEvents();
}

void Engine::Cleanup() {
	Core::Cleanup(objects);
	window.Cleanup();
	Console::Info("Window Closed!");
}

void Engine::Run() {
	InitWindow();
	InitRenderer();
	MainLoop();
	Cleanup();
}