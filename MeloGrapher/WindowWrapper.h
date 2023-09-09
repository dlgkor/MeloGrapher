#pragma once
#include"CommonHeader.h"
#include"meloData.h"
#include"WndProc.h"

#define MAX_CHILD_WINDOW 5
#define WM_TRAYICON (WM_USER + 1)

class MeloWindow {
private:
	HINSTANCE hInstance;

	BlockWrapper block_wrapper; //block calculator	

	HWND root_hwnd; //hidden father window. show by tray icon
	HWND main_hWnd;
	NOTIFYICONDATA nid;

	CustomWindow root_window;
	CustomWindow main_window; //custom child window

	MeloWndData wnd_data; //data for setlongptr
	MeloRootData root_data;
	MeloMainData main_data;
public:
	MeloWindow(HINSTANCE _hInstance);
	int wnd_main();
	~MeloWindow();
private:
	int set_common_data();
	int set_root_data();
	int set_main_data();
	int create_root();
	int destroy_root(); //destroy tray icon
	int create_main(); //create child window
	int show_root();
	int show_main();
};