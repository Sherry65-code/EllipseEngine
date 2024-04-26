#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>

#include "Core.h"
#include "Error.h"

typedef bool EResult;

typedef struct EInitInfo {
	GLFWwindow* pWindow;
	uint32_t width;
	uint32_t height;
	char* title;
	bool isResizable;
	bool isFullscreen;
} EInitInfo;

void eInitEngine(EInitInfo initInfo);
void eRun();

void _eInitWindow();
void _eInitVulkan();
void _eMainLoop();
void _eCleanup();

#endif