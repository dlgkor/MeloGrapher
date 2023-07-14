#include"WndProc.h"

void PaintProc(MeloWndData* main_data) {
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
		main_data->this_window->w_hbit = CreateCompatibleBitmap(main_data->this_window->w_hdc, crt.right, crt.bottom); //hdc�� ȣȯ�ȴ� ������ ũ���� ��Ʈ�� ����
	}

	hMemDC = CreateCompatibleDC(main_data->this_window->w_hdc); //hdc�� ȣȯ�Ǵ� dc ����
	OldBit = (HBITMAP)SelectObject(hMemDC, main_data->this_window->w_hbit); //hbit�� hmemdc�� ����

	dbrush = CreateSolidBrush(RGB(0, 0, 255));
	FillRect(hMemDC, &crt, dbrush);
	DeleteObject(dbrush);

	SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //������ �߰����� ����

	p_graphic = new Gdiplus::Graphics(hMemDC);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	PaintSpectrum(main_data, p_graphic);

	delete p_graphic;

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	if (main_data->this_window->w_hbit) CustomWindow::DrawBitmap(main_data->this_window->w_hdc, 0, 0, main_data->this_window->w_hbit); //hbit�� hdc�� �̿��� ���
}

//display spectrum
void PaintSpectrum(MeloWndData* main_data, Gdiplus::Graphics* p_graphic) {
	if (main_data->block_wrapper->encoded_audio != nullptr) {
		main_data->spectrum_option.base_frequency =
			(double)main_data->block_wrapper->encoded_audio->get_sample_rate() / (double)main_data->spectrum_option.s_window;

		SpectrumBlock* spectrum_instance = new SpectrumBlock();
		main_data->block_wrapper->buffer_manager.check_spectrum_block();
		if (main_data->block_wrapper->buffer_manager.get_spectrum_block(spectrum_instance) != -1) {
			//get current spectrum data
			melo::PrintCircularFrequencyWithGDI(p_graphic, spectrum_instance, main_data->spectrum_option);
		}
		delete spectrum_instance;
	}
}