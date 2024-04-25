#include <Engine/Engine.hpp>

int main() {
	Eclipse eclipse;

	Eclipse::ECreateInfo createInfo{};
	createInfo.pWindow = nullptr;
	createInfo.height = 800;
	createInfo.width = 1000;
	createInfo.title = "Eclipse Engine Demo";
	createInfo.isFullscreen = false;
	createInfo.isResizable = false;

	if (eclipse.eInitEngine(createInfo) != E_SUCCESS)
		eclipse.eThrowError("Could Not Initialize Engine!");

	eclipse.eRun();
}