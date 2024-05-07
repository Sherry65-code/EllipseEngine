#include "Console.hpp"

void Console::Info(std::string info) {
	std::cout << "INFO: " << info << std::endl;
}

void Console::Error(std::string error) {
	std::cerr << "ERROR: " << error << std::endl;
	exit(EXIT_FAILURE);
}

void Console::Warning(std::string warning) {
	std::cout << "WARNING: " << warning << std::endl;
}

void Console::Debug(std::string debug) {
	std::cout << "DEBUG: " << debug << std::endl;
}