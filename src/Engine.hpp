#include "VkCore.hpp"

class Engine {
    public:
    void setDebugMode(bool debugMode);
    void run();

    private:
    Window::WINDOW pWindow;

    void initWindow();
    void initVulkan() const;
    void mainLoop();
    void cleanup();
};