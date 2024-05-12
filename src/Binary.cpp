#include "Engine.hpp"

int main(int argc, char* argv[]) {

    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());

    Engine engine;

    for (uint32_t i = 0; i < args.size(); i++) {
        if (strcmp(args[i].c_str(), "-debug") == 0) {
            engine.setDebugMode(true);
        }
    }

    engine.run();

    return 0;
}