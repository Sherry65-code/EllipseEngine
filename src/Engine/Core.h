#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Error.h"
#include "TypeDefs.h"

void _ePassWindowPointer(GLFWwindow* window);

void _eCreateInstance();
void _eCoreCleanup();

#endif