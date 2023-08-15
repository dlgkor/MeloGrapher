#pragma once
#include"CommonHeader.h"
#include"vector2d.h"

#define MAX_SPECTURM_DISPLAY 500

namespace melo {
	const cpx img(0.0, 1.0);

	class SpectrumOption {
	public:
		int s_window;
		int s_window_half;

		int s_gap;

		double base_frequency;

		double min_out_frequency;
		double max_out_frequency;

		double max_height;

		int n_graph;

		double lenght; //plane width if line spectrum. circle round if circular spectrum

		//line spectrum
		vector2d s_point;


		//circular spectrum
		vector2d r_center;
		double radius;

		vector2d p1[MAX_SPECTURM_DISPLAY];
		vector2d p2[MAX_SPECTURM_DISPLAY];
		vector2d tanline[MAX_SPECTURM_DISPLAY];
		//spectrum line start points. used when displaying circle or line spectrum
	public:
		SpectrumOption();
		int load(const char* filename);
		int save(const char* filename);
		void set_circle_points();
		//void set_line_points();
		~SpectrumOption();
	};
}