#pragma once
#include"CommonHeader.h"
#include"CustomWindow.h"
#include"BlockWrapper.h"
#include"displaySpectrum.h"

#define MAX_CHILD_WINDOW 5

LRESULT CALLBACK WndProc_Hidden(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

struct MeloWndData {
	BlockWrapper* block_wrapper;
	CustomWindow* this_window;
};

class MeloWindow {
public:
	BlockWrapper block_wrapper; //block calculator	
	
	HWND father_wnd; //hidden father window. show by tray icon

	CustomWindow child_window[MAX_CHILD_WINDOW]; //custom child window
	MeloWndData melo_wnd_data[MAX_CHILD_WINDOW]; //data for setlongptr
	int count_child_window;


public:
	MeloWindow();
	int create_melo_child(); //create child window
	~MeloWindow();
};

LRESULT CALLBACK WndProc_Hidden(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
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

LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
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
