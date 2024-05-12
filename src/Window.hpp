#include <GLFW/glfw3.h>

class Window {
    public:
    static GLFWwindow* createWindow(int width, int height, const char* title, bool isResizable, bool isFullscreen);
    static void destroyWindow(GLFWwindow* window);
    static void mainLoop(GLFWwindow* window);
    static void getInstanceExtensions(uint32_t& extensionCount, const char**& extensions);
};