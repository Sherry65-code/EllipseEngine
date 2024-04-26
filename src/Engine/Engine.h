#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define nullptr NULL
#define E_SUCCESS true
#define E_FAILURE false

typedef GLFWwindow* EWindow;
typedef bool EResult;

typedef struct EInitInfo {
	EWindow pWindow;
	uint32_t width;
	uint32_t height;
	char* title;
	bool isResizable;
	bool isFullscreen;
} EInitInfo;

void eInitEngine(EInitInfo initInfo);
void eThrowError(char* error);
void eRun();

void _eInitWindow();
void _eInitVulkan();
void _eMainLoop();