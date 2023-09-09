#include"displaySpectrum.h"



void melo::HEllipse(Gdiplus::Graphics* p_graphic, vector2d center, double rad) {
	Gdiplus::SolidBrush solidbrush(Gdiplus::Color(225,225,225));
	
	Gdiplus::Pen solidpen(Gdiplus::Color(0, 0, 0), 1);

	Gdiplus::Rect solid_circle(center.x - rad, center.y - rad, rad * 2, rad * 2);
	p_graphic->FillEllipse(&solidbrush, solid_circle);
	p_graphic->DrawEllipse(&solidpen, solid_circle);

	//Arc_Design(p_graphic, center, rad - 25);
}

void melo::Arc_Design(Gdiplus::Graphics* p_graphic, vector2d center, double rad) {
	Gdiplus::Rect Arc_rect(center.x - rad, center.y - rad, rad * 2, rad * 2);

	double s_rad = rad - 10;
	Gdiplus::Rect s_Arc_rect(center.x - s_rad, center.y - s_rad, s_rad * 2, s_rad * 2);

	Gdiplus::SolidBrush solidbrush(Gdiplus::Color(255, 255, 255));
	Gdiplus::SolidBrush nonebrush(Gdiplus::Color(0, 0, 255));
	Gdiplus::Pen solidpen(Gdiplus::Color(0, 0, 0), 0.1);

	for (int i = 0; i < 3; i++) {
		int start_rad = 120 * i;
		p_graphic->FillPie(&solidbrush, Arc_rect, start_rad, 60);
		p_graphic->DrawPie(&solidpen, Arc_rect, start_rad, 60);
		p_graphic->FillPie(&nonebrush, s_Arc_rect, start_rad - 30, 120);
		p_graphic->DrawArc(&solidpen, s_Arc_rect, start_rad, 60);
	}
}

int melo::PrintCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, SpectrumBlock* spectrum_block, SpectrumOption option) {
	Gdiplus::Pen* p_pen;
	Gdiplus::SolidBrush solidbrush1(Gdiplus::Color(254, 83, 187));

	if (spectrum_block->get_total_size() == 0)
		return 0;

	int maxFqpoint = (int)(option.max_out_frequency / option.base_frequency);
	int minFqpoint = (int)(option.min_out_frequency / option.base_frequency);
	if (maxFqpoint == 0)
		return 0;

	int Cut = (maxFqpoint - minFqpoint) / option.n_graph;
	if (Cut == 0)
		return 0;

	double amp = 0.1;

	double* f = new double[option.n_graph];
	for (int i = 0; i < option.n_graph; i++) {
		f[i] = 0.0f;
	}

	int ic = 0;

	const double* output = spectrum_block->get_data();

	for (int i = 0; i < option.n_graph * Cut; i++) {
		ic = i / Cut;
		f[ic] = max(f[ic], output[i + minFqpoint] * amp);

		if (f[ic] > option.max_height) {
			f[ic] = option.max_height;
		}
		else if (f[ic] < 0) {
			f[ic] = 0;
		}
	}

	cpx W = exp((-2.0 * M_PI * img) / (double)option.n_graph);
	cpx Z(0, 1);

	p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 0.1);

	for (int i = 0; i < option.n_graph; i++) {
		Gdiplus::Point p_point[] = {
			Gdiplus::Point((INT)(option.r_center.x + option.p1[i].x), -(INT)(option.r_center.y + option.p1[i].y)),
			Gdiplus::Point((INT)(option.r_center.x + option.p1[i].x + (f[i] * option.tanline[i].x)), -(INT)(option.r_center.y + option.p1[i].y + (f[i] * option.tanline[i].y))),
			Gdiplus::Point((INT)(option.r_center.x + option.p2[i].x + (f[i] * option.tanline[i].x)), -(INT)(option.r_center.y + option.p2[i].y + (f[i] * option.tanline[i].y))),
			Gdiplus::Point((INT)(option.r_center.x + option.p2[i].x), -(INT)(option.r_center.y + option.p2[i].y))
		};
		p_graphic->FillPolygon(&solidbrush1, p_point, 4);
		p_graphic->DrawPolygon(p_pen, p_point, 4);
	}

	delete p_pen;

	delete[] f;
	return 1;
}