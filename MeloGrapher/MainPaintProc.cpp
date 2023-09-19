#include"WndProc.h"

void PaintProc_Root(MeloGraphData* root_data) {
	RECT crt;

	Gdiplus::Graphics* memGraphics;

	crt.left = 0;
	crt.top = 0;
	crt.right = root_data->this_window->screenWidth;
	crt.bottom = root_data->this_window->screenHeight;

	memGraphics = new Gdiplus::Graphics(root_data->this_window->gdi_bitmap);
	memGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	memGraphics->Clear(Gdiplus::Color(255, 255, 255));

	memGraphics->TranslateTransform(crt.right / 2, crt.bottom / 2);

	PaintSpectrum(root_data->common_data, memGraphics);

	delete memGraphics;

	Gdiplus::Graphics graphics(root_data->this_window->w_hdc); //Double Buffering
	graphics.DrawImage(root_data->this_window->gdi_bitmap, 0, 0);
}

void PaintProc_Main(MeloMainData* main_data) {
	RECT crt;

	Gdiplus::Graphics* memGraphics;

	crt.left = 0;
	crt.top = 0;
	crt.right = main_data->this_window->screenWidth;
	crt.bottom = main_data->this_window->screenHeight;

	memGraphics = new Gdiplus::Graphics(main_data->this_window->gdi_bitmap);
	memGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	memGraphics->Clear(Gdiplus::Color(200, 200, 200));

	memGraphics->TranslateTransform(crt.right / 2, crt.bottom / 2);

	//PaintMenu(&main_data->custom_menu, memGraphics);

	delete memGraphics;

	Gdiplus::Graphics graphics(main_data->this_window->w_hdc); //Double Buffering
	graphics.DrawImage(main_data->this_window->gdi_bitmap, 0, 0);

	//Display by UpdateLayeredWindow
}

//display spectrum
void PaintSpectrum(MeloWndData* common_data, Gdiplus::Graphics* p_graphic) {
	if (common_data->block_wrapper->encoded_audio != nullptr) {
		common_data->spectrum_option.base_frequency =
			(double)common_data->block_wrapper->encoded_audio->get_sample_rate() / (double)common_data->spectrum_option.s_window;

		SpectrumBlock* spectrum_instance = new SpectrumBlock();
		common_data->block_wrapper->buffer_manager.check_spectrum_block();

		if (common_data->block_wrapper->buffer_manager.get_spectrum_block(spectrum_instance) != -1) {
			//get current spectrum data
			melo::PrintCircularFrequencyWithGDI(p_graphic, spectrum_instance, common_data->spectrum_option);
			//melo::HEllipse(p_graphic, common_data->spectrum_option.r_center, common_data->spectrum_option.radius);
		}
		delete spectrum_instance;
	}
}

void PaintMenu(CustomUI::Menu* menu, Gdiplus::Graphics* p_graphic) {
	menu->Render(p_graphic, 0.01f);
}