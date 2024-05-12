#pragma once

#include <fmt/color.h>

class Console {
public:
    template<typename... Args>
    static void PrintError(const std::string format, Args... args) {
        fmt::print(fmt::fg(fmt::color::red), "ERROR: ");
        fmt::print(fmt::runtime(format), args...);
        fmt::print("\n");
        exit(-1);
    }

    template<typename... Args>
    static void PrintInfo(const std::string format, Args... args) {
        fmt::print(fmt::fg(fmt::color::green), "INFO: ");
        fmt::print(fmt::runtime(format), args...);
        fmt::print("\n");
    }

    template<typename... Args>
    static void PrintWarning(const std::string format, Args... args) {
        fmt::print(fmt::fg(fmt::color::orange), "WARNING: ");
        fmt::print(fmt::runtime(format), args...);
        fmt::print("\n");
    }

    template<typename... Args>
    static void PrintDebug(const std::string format, Args... args) {
        fmt::print(fmt::fg(fmt::color::yellow), "DEBUG: ");
        fmt::print(fmt::runtime(format), args...);
        fmt::print("\n");
    }
};