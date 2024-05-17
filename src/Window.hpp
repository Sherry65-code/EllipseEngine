#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* pWindow;
public:
    typedef GLFWwindow* WINDOW;

    WINDOW createWindow(int width, int height, const char* title, bool isResizable, bool isFullscreen);
    bool shouldWindowClose();
    void pollEvents();
    void destroyWindow();
    
    static void getInstanceExtensions(uint32_t& extensionCount, const char**& extensions);
};