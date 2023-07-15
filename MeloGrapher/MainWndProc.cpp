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
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 255), 255, LWA_ALPHA | LWA_COLORKEY);
		SetTimer(hWnd, 1, 10, NULL);
		return 0;
	case WM_MOUSEMOVE:
		MenuMouseMove(&main_data->custom_menu, vector2d((double)(LOWORD(lParam) - (main_data->this_window->screenWidth / 2)), 
			(double)(HIWORD(lParam) - (main_data->this_window->screenHeight / 2))));
		return 0;
	case WM_LBUTTONDOWN:
		MenuMouseClick(&main_data->custom_menu, vector2d((double)(LOWORD(lParam) - (main_data->this_window->screenWidth / 2)),
			(double)(HIWORD(lParam) - (main_data->this_window->screenHeight / 2))));
		return 0;
	case WM_LBUTTONUP:
		MenuMouseRelease(&main_data->custom_menu, vector2d((double)(LOWORD(lParam) - (main_data->this_window->screenWidth / 2)),
			(double)(HIWORD(lParam) - (main_data->this_window->screenHeight / 2))));
		return 0;
	case WM_COMMAND:
		CommandProc(main_data, wParam);
		return 0;
	case WM_PAINT:
		main_data->this_window->w_hdc = BeginPaint(hWnd, &main_data->this_window->w_ps);
		PaintProc(main_data);
		EndPaint(hWnd, &main_data->this_window->w_ps);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		return 0;
	case WM_DESTROY:
		main_data->block_wrapper->close_file();
		PostMessage(main_data->root_hwnd, WM_DESTROY, NULL, NULL);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}