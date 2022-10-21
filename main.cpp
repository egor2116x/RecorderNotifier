#include "MainWindow.h"
#include <tchar.h>

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	MainWindow::getInstance().run(instance);
	return 0;
}