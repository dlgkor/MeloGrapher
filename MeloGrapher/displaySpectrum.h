#pragma once
#include"CommonHeader.h"
#include"spectrumUtils.h"
#include"DataStruct.h"

#define M_PI 3.141592

namespace melo {
	const cpx img(0.0, 1.0);

	void HEllipse(HDC hdc, vector2d center, double rad) {
		Ellipse(hdc, center.x - rad, center.y - rad, center.x + rad, center.y + rad);
	}

	int PrintCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, SpectrumBlock* spectrum_block, SpectrumOption option) {
		Gdiplus::Pen* p_pen;
		Gdiplus::SolidBrush solidbrush1(Gdiplus::Color(255, 120, 193, 243));

		if (spectrum_block->get_total_size() == 0)
			return 0;

		int maxFqpoint = (int)(option.max_out_frequency / option.base_frequency);
		int minFqpoint = (int)(option.min_out_frequency / option.base_frequency);
		if (maxFqpoint == 0)
			return 0;

		int Cut = (maxFqpoint - minFqpoint) / option.n_graph;
		if (Cut == 0)
			return 0;

		double amp = 15.0;

		double* f = new double[option.n_graph];
		for (int i = 0; i < option.n_graph; i++) {
			f[i] = 0.0f;
		}

		int ic = 0;

		const double* output = spectrum_block->get_data();

		for (int i = 0; i < option.n_graph * Cut; i++) {
			ic = i / Cut;
			amp = 0.2;
			f[ic] = max(f[ic], output[i + minFqpoint] * amp);

			if (ic > option.max_height) {
				f[ic] = option.max_height;
			}
			else if (f[ic] < 0) {
				f[ic] = 0;
			}
		}

		cpx W = exp((-2.0 * M_PI * img) / (double)option.n_graph);
		cpx Z(0, 1);
		vector2d p1, p2;
		vector2d tanline;

		p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 1);

		Gdiplus::Point p_point[500];

		for (int i = 0; i < option.n_graph; i++) {
			p1.x = option.radius * Z.real(); p1.y = option.radius * Z.imag();
			Z *= W;
			p2.x = option.radius * Z.real(); p2.y = option.radius * Z.imag();
			tanline = ((p1 + p2) * 0.5).UnitVector();

			Gdiplus::Point p_point[] = {
				Gdiplus::Point((INT)(option.r_center.x + p1.x), -(INT)(option.r_center.y + p1.y)),
				Gdiplus::Point((INT)(option.r_center.x + p1.x + (f[i] * tanline.x)), -(INT)(option.r_center.y + p1.y + (f[i] * tanline.y))),
				Gdiplus::Point((INT)(option.r_center.x + p2.x + (f[i] * tanline.x)), -(INT)(option.r_center.y + p2.y + (f[i] * tanline.y))),
				Gdiplus::Point((INT)(option.r_center.x + p2.x), -(INT)(option.r_center.y + p2.y))
			};
			p_graphic->FillPolygon(&solidbrush1, p_point, 4);
			p_graphic->DrawPolygon(p_pen, p_point, 4);
		}

		delete p_pen;

		delete[] f;
		return 1;
	}
}