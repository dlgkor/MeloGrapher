#include"CustomWindow.h"

void CustomWindow::DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

CustomWindow::CustomWindow() {
	screenWidth = CW_USEDEFAULT; screenHeight = CW_USEDEFAULT;
	locationX = CW_USEDEFAULT; locationY = CW_USEDEFAULT;

	w_hWnd = NULL;
	w_hbit = NULL;
}

void CustomWindow::setScreenSize(int _sizeW, int _sizeH) {
	screenWidth = _sizeW;
	screenHeight = _sizeH;

}

void CustomWindow::SetScreenLocation(int _locateX, int _locateY) {
	locationX = _locateX;
	locationY = _locateY;

}

void CustomWindow::Apply() {
	//apply and show change window
	MoveWindow(w_hWnd, locationX, locationY, screenWidth, screenHeight, TRUE);
	if (w_hbit)
		DeleteObject(w_hbit);

	w_hdc = GetDC(w_hWnd);
	w_hbit = CreateCompatibleBitmap(w_hdc, screenWidth, screenHeight);
	ReleaseDC(w_hWnd, w_hdc);
}

void CustomWindow::registerDefaultWndClass(HINSTANCE hInstance, WNDPROC WndProc, LPCWSTR lpszClass) {
	WNDCLASS WndClass;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
}

HWND CustomWindow::CreateCustomWindow(HINSTANCE hInstance, WNDPROC WndProc, LPCWSTR lpszClass) {
	registerDefaultWndClass(hInstance, WndProc, lpszClass);

	w_hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		locationX, locationY, screenWidth, screenHeight,
		NULL, (HMENU)NULL, hInstance, NULL);

	return w_hWnd;
}

CustomWindow::~CustomWindow() {
	if (w_hbit)
		DeleteObject(w_hbit);
}