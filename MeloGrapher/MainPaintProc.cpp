#include"WndProc.h"

void PaintProc_Old(MeloWndData* main_data) {
	//Init hbit(Double buffering)
	//Draw RGB(0,0,255) Rect at hbit

	//Drawing...

	//Make background in circle to have little transparency
	//Draw it with gdi+

	//Draw hbit at screen
	//Destroy hbit

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

	//dbrush = CreateSolidBrush(RGB(0, 0, 255));
	//FillRect(hMemDC, &crt, dbrush);
	//DeleteObject(dbrush);

	SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //원점을 중간으로 설정

	//Gdiplus::SolidBrush solidbrush(Gdiplus::Color(0, 0, 255));

	p_graphic = new Gdiplus::Graphics(hMemDC);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	p_graphic->Clear(Gdiplus::Color(0, 0, 255));


	PaintSpectrum(main_data, p_graphic);
	PaintMenu(&main_data->custom_menu, p_graphic);

	delete p_graphic;

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	if (main_data->this_window->w_hbit) CustomWindow::DrawBitmap(main_data->this_window->w_hdc, 0, 0, main_data->this_window->w_hbit); //hbit을 hdc를 이용해 출력
}

void PaintProc(MeloWndData* main_data) {
	//Init hbit(Double buffering)
	//Draw RGB(0,0,255) Rect at hbit

	//Drawing...

	//Make background in circle to have little transparency
	//Draw it with gdi+

	//Draw hbit at screen
	//Destroy hbit

	RECT crt;

	Gdiplus::Graphics* p_graphic;
	Gdiplus::Pen* p_pen;

	//GetWindowRect(hWnd, &crt);
	crt.left = 0;
	crt.top = 0;
	crt.right = main_data->this_window->screenWidth;
	crt.bottom = main_data->this_window->screenHeight;

	if (main_data->this_window->gdi_bitmap == nullptr) {
		main_data->this_window->gdi_bitmap = new Gdiplus::Bitmap(crt.right, crt.bottom);
	}

	p_graphic = new Gdiplus::Graphics(main_data->this_window->gdi_bitmap);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	p_graphic->Clear(Gdiplus::Color(0, 0, 255));

	p_graphic->TranslateTransform(crt.right / 2, crt.bottom / 2);

	PaintSpectrum(main_data, p_graphic);
	PaintMenu(&main_data->custom_menu, p_graphic);

	delete p_graphic;

	Gdiplus::Graphics graphics(main_data->this_window->w_hdc);
	graphics.DrawImage(main_data->this_window->gdi_bitmap, 0, 0);
}

//display spectrum
void PaintSpectrum(MeloWndData* main_data, Gdiplus::Graphics* p_graphic) {

	if (main_data->block_wrapper->encoded_audio != nullptr) {
		main_data->spectrum_option.base_frequency =
			(double)main_data->block_wrapper->encoded_audio->get_sample_rate() / (double)main_data->spectrum_option.s_window;

		SpectrumBlock* spectrum_instance = new SpectrumBlock();
		if (main_data->block_wrapper->buffer_manager.check_spectrum_block() != -1) {
			melo::HEllipse(p_graphic, main_data->spectrum_option.r_center, main_data->spectrum_option.radius);
		}

		if (main_data->block_wrapper->buffer_manager.get_spectrum_block(spectrum_instance) != -1) {
			//get current spectrum data
			melo::PrintCircularFrequencyWithGDI(p_graphic, spectrum_instance, main_data->spectrum_option);
		}
		delete spectrum_instance;
	}
}

void PaintMenu(CustomUI::Menu* menu, Gdiplus::Graphics* p_graphic) {
	menu->Render(p_graphic, 0.02f);
}