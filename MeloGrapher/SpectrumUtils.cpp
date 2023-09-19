#include"spectrumUtils.h"

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

void melo::SpectrumOption::set_circle_points() {
	cpx W = exp((-2.0 * M_PI * img) / (double)n_graph);
	cpx Z(0, 1);

	for (int i = 0; i < n_graph; i++) {
		p1[i].x = radius * Z.real(); p1[i].y = radius * Z.imag();
		Z *= W;
		p2[i].x = radius * Z.real(); p2[i].y = radius * Z.imag();
		tanline[i] = ((p1[i] + p2[i]) * 0.5).UnitVector();

		//set points and tanline
	}
}

void melo::SpectrumOption::set_linear_points() {
	double gap = lenght / n_graph;
	for (int i = 0; i < n_graph; i++) {
		p1[i].x = s_point.x + gap * i; p1[i].y = s_point.y;
		p2[i].x = s_point.x + gap * (i+1); p2[i].y = s_point.y; 
		tanline[i] = vector2d(0, 1);
		//set points and tanline
	}
}

melo::SpectrumOption::~SpectrumOption() {

}