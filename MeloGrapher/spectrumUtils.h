#pragma once
#include"common.h"
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

		double lenght;
		//plane width if line spectrum. circle round if circular spectrum

		//line spectrum
		vector2d s_point;


		//circular spectrum
		vector2d r_center;
		double radius;
	};

	class SpectrumImage {
	public:
		std::vector<cpx> data;
		int s_location; //location relative to one audio data buffer
	};

	typedef std::vector<SpectrumImage> SpectrumVideo;

	/*
	class SpectrumVideo {
	public:
		std::vector<SpectrumImage> data;
		int cur;
	public:
		void SpectrumVide() {
			cur = 0;
		}
		void resetCursor() {
			cur = 0;
		}
		void ClearData() {
			data.clear();
			resetCursor();
		}
	};
	*/
}