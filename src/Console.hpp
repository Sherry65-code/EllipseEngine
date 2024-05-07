#pragma once

#include <iostream>

class Console {
public:
	static void Info(std::string info);
	static void Error(std::string error);
	static void Warning(std::string warning);
	static void Debug(std::string debug);
};