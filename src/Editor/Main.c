#include <Engine/Engine.h>

int main(int argc, char* argv[]) {
	EInitInfo initInfo;
	initInfo.pWindow = nullptr;
	initInfo.width = 800;
	initInfo.height = 600;
	initInfo.title = "Eclipse Engine";
	initInfo.isFullscreen = false;
	initInfo.isResizable = false;

	eInitEngine(initInfo);
	eRun();
}