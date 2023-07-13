#include"CommonHeader.h"
#include"WindowWrapper.cpp"

//확인할 수 없는 외부참조 오류는 cpp파일을 include 하니 해결되었다

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	MeloWindow melo_window(hInstance);
	return melo_window.wnd_main();
}