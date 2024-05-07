#pragma once

// Options - VULKAN
#define API VULKAN

#if API == VULKAN
#include "VkCore.hpp"
#else
#endif

#include "Window.hpp"

class Engine {
private:
	void InitWindow();
	void InitRenderer();
	void MainLoop();
	void Cleanup();
public:
	void Run();
};