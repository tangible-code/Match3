
#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"

int WinWidth = 960;
int WinHeight = 540;


int WinMain(int argc, char* args[]) {

	if (!Application::Initialize("Match", 960, 540))
	{
		return -1;
	}
	else
	{
		Application::Run();
	}
	return 0;
}
