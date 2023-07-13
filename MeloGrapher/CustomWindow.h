#pragma once
#include"CommonHeader.h"

class CustomWindow {
public:
	int screenWidth, screenHeight;
	int locationX, locationY;
	HWND w_hWnd;

	HDC w_hdc, w_hMemDC;
	PAINTSTRUCT w_ps;

	HBITMAP w_hbit, w_oldbit;
public:
	CustomWindow();
	void setScreenSize(int _sizeW, int _sizeH); //update screen size
	void SetScreenLocation(int _locateX, int _locateY); //update screen location
	void Apply();
	void registerDefaultWndClass(HINSTANCE hInstance, WNDPROC WndProc, LPCWSTR lpszClass); //register wndclass
	HWND CreateCustomWindow(HINSTANCE hInstance, WNDPROC WndProc, LPCWSTR lpszClass); //create window with no icon
	~CustomWindow();

	static void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);
};