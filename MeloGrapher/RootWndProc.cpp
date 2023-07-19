#include"WndProc.h"

LRESULT CALLBACK WndProc_Root(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	MeloRootWndData* root_data = reinterpret_cast<MeloRootWndData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
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