#include"CommonHeader.h"
#include"WindowWrapper.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	MeloWindow melo_window(hInstance);
	return melo_window.wnd_main();
}