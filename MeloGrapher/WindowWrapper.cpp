#include"WindowWrapper.h"

MeloWindow::MeloWindow(HINSTANCE _hInstance) {
	hInstance = _hInstance;
}

//create root wnd
int MeloWindow::create_root() {
	LPCTSTR lpszClass = TEXT("meloGrapher Root");

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.lpfnWndProc = WndProc_Root;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClass;
	RegisterClassEx(&wcex);

	root_hwnd = CreateWindowEx(0, lpszClass, L"MeloGrapher", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, NULL, NULL, hInstance, NULL);

	//initialize root data
	root_data.block_wrapper = &block_wrapper;
	SetWindowLongPtr(root_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&root_data));

	//ShowWindow(root_hwnd, SW_HIDE);

	nid = { sizeof(NOTIFYICONDATA) };
	nid.hWnd = root_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	wcscpy_s(nid.szTip, L"MeloGrapher Tray Icon");

	Shell_NotifyIcon(NIM_ADD, &nid);

	return 0;
}

//destroy tray icon of root wnd
int MeloWindow::destroy_root() {
	Shell_NotifyIcon(NIM_DELETE, &nid);

	return 0;
}

int MeloWindow::create_child() {

	return 0;
}

int MeloWindow::show_child() {

	return 0;
}

//final wrapper
int MeloWindow::wnd_main() {
	MSG Message;

	create_root();

	create_child();
	show_child();

	//translate and dispatch message
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	destroy_root();

	return (int)Message.wParam;
}

MeloWindow::~MeloWindow() {

}







LRESULT CALLBACK WndProc_Root(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


LRESULT CALLBACK WndProc_Child(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	//SetWindowLongPtr from wndmain or MeloWindow class
	//GetWindowLongPtr from here
	//get speciic pointer by handle data
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
