#include "Engine.hpp"

const std::string window_title = "Eclipse Engine Demo";
const uint32_t window_width = 800;
const uint32_t window_height = 600;
const bool window_is_fullscreen = false;
const bool window_is_resizable = false;

Window window;
Console console;

Window::WINDOW gWindow = nullptr;

void Engine::InitWindow() {
	
	uint8_t window_return_value = window.NewWindow(window_title, window_width, window_height, window_is_fullscreen, window_is_resizable);
	
	switch (window_return_value) {
		case 0: console.Info("Created Window Successfully!"); break;
		case 1: console.Error("Failed to initialize window!"); break;
		case 2: console.Error("Failed to create window!"); break;
	}

	gWindow = window.Return();

}

void Engine::InitRenderer() {

}

void Engine::MainLoop() {
	while (!glfwWindowShouldClose(gWindow))
		glfwPollEvents();
}

void Engine::Cleanup() {

	window.Cleanup();
	console.Info("Window Closed!");
}

void Engine::Run() {
	InitWindow();
	InitRenderer();
	MainLoop();
	Cleanup();
}