#pragma once
#include"CommonHeader.h"
#include"BlockWrapper.h"
#include"displaySpectrum.h"
#include"CustomWindow.h"

#define MAX_CHILD_WINDOW 5
#define WM_TRAYICON (WM_USER + 1)

LRESULT CALLBACK WndProc_Root(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc_Child(HWND, UINT, WPARAM, LPARAM);

struct MeloRootWndData {
	BlockWrapper* block_wrapper;
};


struct MeloWndData {
	int wnd_id;
	BlockWrapper* block_wrapper;
	CustomWindow* this_window;
	melo::SpectrumOption spectrum_option;
};

class MeloWindow {
public:
	HINSTANCE hInstance;

	BlockWrapper block_wrapper; //block calculator	
	
	HWND root_hwnd; //hidden father window. show by tray icon
	MeloRootWndData root_data;
	NOTIFYICONDATA nid;

	CustomWindow child_window; //custom child window
	MeloWndData child_data; //data for setlongptr
	//int count_child_window;
public:
	MeloWindow(HINSTANCE _hInstance);
	int create_root();
	int destroy_root();
	int create_child(); //create child window
	int show_child();
	int wnd_main();
	~MeloWindow();
};

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