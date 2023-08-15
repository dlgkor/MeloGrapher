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

melo::SpectrumOption::~SpectrumOption() {

}