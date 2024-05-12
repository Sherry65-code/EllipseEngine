#include "VkCore.hpp"

class Engine {
    public:
    void setDebugMode(bool debugMode);
    void run();

    private:
    GLFWwindow* pWindow;

    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
};