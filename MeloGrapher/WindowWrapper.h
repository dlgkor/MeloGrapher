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
	int destroy_root(); //destroy tray icon
	int create_child(); //create child window
	int show_child();
	int wnd_main();
	~MeloWindow();
};