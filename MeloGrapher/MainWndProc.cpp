#include"WndProc.h"
/*
* SetWindowLongPtr from wndmain or MeloWindow class
* GetWindowLongPtr from here
* get speciic pointer by handle data
*/
LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	MeloWndData* main_data = reinterpret_cast<MeloWndData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));


	switch (iMessage) {
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		main_data->this_window->w_hdc = BeginPaint(hWnd, &main_data->this_window->w_ps);
		//Init hbit(Double buffering)
		//Draw RGB(0,0,255) Rect at hbit

		//Drawing...

		//Make background in circle to have little transparency
		//Draw it with gdi+

		//Draw hbit at screen
		//Destroy hbit
		EndPaint(hWnd, &main_data->this_window->w_ps);
		return 0;
	case WM_DESTROY:
		PostMessage(main_data->root_hwnd, WM_DESTROY, NULL, NULL);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}