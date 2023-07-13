#pragma once
#include"CommonHeader.h"
#include"BlockWrapper.h"
#include"displaySpectrum.h"
#include"CustomWindow.h"

#define MAX_CHILD_WINDOW 5
#define WM_TRAYICON (WM_USER + 1)

LRESULT CALLBACK WndProc_Root(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc_Main(HWND, UINT, WPARAM, LPARAM);

struct MeloRootWndData {
	BlockWrapper* block_wrapper; 
	//no calculation
	//just check child window state and display
};

struct MeloWndData {
	HWND root_hwnd; //use to post destroy message to root when child is distroyed
	BlockWrapper* block_wrapper;
	CustomWindow* this_window;
	melo::SpectrumOption spectrum_option;
};

class MeloWindow {
private:
	HINSTANCE hInstance;

	BlockWrapper block_wrapper; //block calculator	

	HWND root_hwnd; //hidden father window. show by tray icon
	MeloRootWndData root_data;
	NOTIFYICONDATA nid;

	CustomWindow main_window; //custom child window
	MeloWndData main_data; //data for setlongptr
public:
	MeloWindow(HINSTANCE _hInstance);
	int wnd_main();
	~MeloWindow();
private:
	int set_root_data();
	int set_main_data();
	int create_root();
	int destroy_root(); //destroy tray icon
	int create_main(); //create child window
	int show_main();
};