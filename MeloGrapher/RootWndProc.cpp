#include"WndProc.h"

LRESULT CALLBACK WndProc_Root(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	MeloRootData* root_data = reinterpret_cast<MeloRootData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 10, NULL); //change to 17
		return 0;
	case WM_PAINT:
		root_data->this_window->w_hdc = BeginPaint(hWnd, &root_data->this_window->w_ps);
		PaintProc_Root(root_data);
		EndPaint(hWnd, &root_data->this_window->w_ps);
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
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}