#pragma once
#include"CommonHeader.h"
#include"vector2d.h"

namespace melo {

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
	public:
		SpectrumOption();
		int load(const char* filename);
		int save(const char* filename);
		~SpectrumOption();
	};
}


melo::SpectrumOption::SpectrumOption() {

}

int melo::SpectrumOption::load(const char* filename) {
	//load audio spectrum option
	return 0;
}

int melo::SpectrumOption::save(const char* filename) {
	//save audio spectrum option
	return 0;
}

melo::SpectrumOption::~SpectrumOption() {

}