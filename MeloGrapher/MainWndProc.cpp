#include"WndProc.h"
/*
* SetWindowLongPtr from wndmain or MeloWindow class
* GetWindowLongPtr from here
* get speciic pointer by handle data
*/

LRESULT CALLBACK WndProc_Main(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	MeloWndData* main_data = reinterpret_cast<MeloWndData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	const char* filename = "C:/Users/ydhan/dlg project/mp3/Hit the Floor.mp3";

	switch (iMessage) {
	case WM_CREATE:
		return 0;
	case WM_LBUTTONDOWN:
		main_data->block_wrapper->close_file(); //close if file is opened
		main_data->block_wrapper->open_file(filename);
		main_data->block_wrapper->fill_thread();
		main_data->block_wrapper->play_file();

		SetTimer(hWnd, 1, 10, NULL);
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

		paintFunction(main_data);

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
		if (main_data->this_window->w_hbit) DeleteObject(main_data->this_window->w_hbit);
		PostMessage(main_data->root_hwnd, WM_DESTROY, NULL, NULL);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


void paintFunction(MeloWndData* main_data) {
	HDC hMemDC;
	HBITMAP OldBit;
	HBRUSH dbrush;
	RECT crt;

	Gdiplus::Graphics* p_graphic;
	Gdiplus::Pen* p_pen;

	//GetWindowRect(hWnd, &crt);
	crt.left = 0;
	crt.top = 0;
	crt.right = main_data->this_window->screenWidth;
	crt.bottom = main_data->this_window->screenHeight;

	if (main_data->this_window->w_hbit == NULL) {
		main_data->this_window->w_hbit = CreateCompatibleBitmap(main_data->this_window->w_hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
	}

	hMemDC = CreateCompatibleDC(main_data->this_window->w_hdc); //hdc와 호환되는 dc 생성
	OldBit = (HBITMAP)SelectObject(hMemDC, main_data->this_window->w_hbit); //hbit을 hmemdc에 선택

	dbrush = CreateSolidBrush(RGB(200, 200, 200));
	FillRect(hMemDC, &crt, dbrush);
	DeleteObject(dbrush);

	SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //원점을 중간으로 설정

	p_graphic = new Gdiplus::Graphics(hMemDC);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	//display spectrum
	if (main_data->block_wrapper->encoded_audio != nullptr) {
		melo::SpectrumOption spectrum_option;
		spectrum_option.s_gap = 32768 / 16;
		spectrum_option.s_window = 32768 / 4;
		spectrum_option.s_window_half = spectrum_option.s_window / 2;
		spectrum_option.base_frequency = (double)main_data->block_wrapper->encoded_audio->get_sample_rate() / (double)spectrum_option.s_window;

		spectrum_option.max_out_frequency = 1000;
		spectrum_option.min_out_frequency = 10;
		spectrum_option.max_height = 400;
		spectrum_option.n_graph = 100;
		spectrum_option.r_center = { 0, 0 };
		spectrum_option.radius = 200;

		SpectrumBlock* spectrum_instance = new SpectrumBlock();
		main_data->block_wrapper->buffer_manager.check_spectrum_block();
		if (main_data->block_wrapper->buffer_manager.get_spectrum_block(spectrum_instance) != -1) {
			//get current spectrum data
			melo::PrintCircularFrequencyWithGDI(p_graphic, spectrum_instance, spectrum_option);
		}
		delete spectrum_instance;
	}

	delete p_graphic;

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	if (main_data->this_window->w_hbit) CustomWindow::DrawBitmap(main_data->this_window->w_hdc, 0, 0, main_data->this_window->w_hbit); //hbit을 hdc를 이용해 출력
}
