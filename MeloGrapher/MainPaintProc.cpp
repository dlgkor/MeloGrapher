#include"WndProc.h"

void PaintProc(MeloWndData* main_data) {
	RECT crt;

	Gdiplus::Graphics* memGraphics;

	crt.left = 0;
	crt.top = 0;
	crt.right = main_data->this_window->screenWidth;
	crt.bottom = main_data->this_window->screenHeight;

	memGraphics = new Gdiplus::Graphics(main_data->this_window->gdi_bitmap);
	memGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	memGraphics->Clear(Gdiplus::Color(0, 0, 255));

	memGraphics->TranslateTransform(crt.right / 2, crt.bottom / 2);

	PaintSpectrum(main_data, memGraphics);
	PaintMenu(&main_data->custom_menu, memGraphics);

	delete memGraphics;

	Gdiplus::Graphics graphics(main_data->this_window->w_hdc); //Double Buffering
	graphics.DrawImage(main_data->this_window->gdi_bitmap, 0, 0);
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
			melo::HEllipse(p_graphic, main_data->spectrum_option.r_center, main_data->spectrum_option.radius);
		}
		delete spectrum_instance;
	}
}

void PaintMenu(CustomUI::Menu* menu, Gdiplus::Graphics* p_graphic) {
	menu->Render(p_graphic, 0.02f);
}