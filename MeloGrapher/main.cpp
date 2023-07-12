#include"CommonHeader.h"
#include"WindowWrapper.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("MeloGrapher");

HBITMAP hbit;

CustomWindow custom_window;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	
	ULONG_PTR gpToken;
	Gdiplus::GdiplusStartupInput gpsi;
	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Gdiplus::Ok) return 0;

	HWND hWnd;
	MSG Message;

	custom_window.setScreenSize(700, 700);
	hWnd = custom_window.CreateCustomWindow(hInstance, WndProc, lpszClass);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	Gdiplus::GdiplusShutdown(gpToken);

	return (int)Message.wParam;
}

void paintFunction(HWND hWnd, HDC hdc, BlockWrapper* final_wrapper) {
	HDC hMemDC;
	HBITMAP OldBit;
	HBRUSH dbrush;
	RECT crt;

	Gdiplus::Graphics* p_graphic;
	Gdiplus::Pen* p_pen;

	//GetWindowRect(hWnd, &crt);
	crt.left = 0;
	crt.top = 0;
	crt.right = 700;
	crt.bottom = 700;

	if (hbit == NULL) {
		hbit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
	}

	hMemDC = CreateCompatibleDC(hdc); //hdc와 호환되는 dc 생성
	OldBit = (HBITMAP)SelectObject(hMemDC, hbit); //hbit을 hmemdc에 선택

	dbrush = CreateSolidBrush(RGB(200, 200, 200));
	FillRect(hMemDC, &crt, dbrush);
	DeleteObject(dbrush);

	SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //원점을 중간으로 설정

	p_graphic = new Gdiplus::Graphics(hMemDC);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	
	//display spectrum
	if (final_wrapper->encoded_audio != nullptr) {
		melo::SpectrumOption spectrum_option;
		spectrum_option.s_gap = 32768 / 16;
		spectrum_option.s_window = 32768 / 4;
		spectrum_option.s_window_half = spectrum_option.s_window / 2;
		spectrum_option.base_frequency = (double)final_wrapper->encoded_audio->get_sample_rate() / (double)spectrum_option.s_window;

		spectrum_option.max_out_frequency = 1000;
		spectrum_option.min_out_frequency = 10;
		spectrum_option.max_height = 400;
		spectrum_option.n_graph = 100;
		spectrum_option.r_center = { 0, 0 };
		spectrum_option.radius = 200;

		SpectrumBlock* spectrum_instance = new SpectrumBlock();
		final_wrapper->buffer_manager.check_spectrum_block();
		if (final_wrapper->buffer_manager.get_spectrum_block(spectrum_instance) != -1) {
			//get current spectrum data
			melo::PrintCircularFrequencyWithGDI(p_graphic, spectrum_instance, spectrum_option);
		}
		delete spectrum_instance;
	}

	delete p_graphic;

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	if (hbit) CustomWindow::DrawBitmap(hdc, 0, 0, hbit); //hbit을 hdc를 이용해 출력
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	const char* filename = "C:/Users/ydhan/dlg project/test/ffmpegtest/test.mp3";
	static BlockWrapper final_wrapper;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		hbit = NULL;

		SetTimer(hWnd, 1, 10, NULL);
		return 0;
	case WM_LBUTTONDOWN:
		final_wrapper.open_file(filename);
		final_wrapper.fill_audio_thread();
		final_wrapper.fill_spectrum_thread();

		final_wrapper.play_file();
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		paintFunction(hWnd, hdc, &final_wrapper);
		
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		final_wrapper.close_file();
		if (hbit) DeleteObject(hbit);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}