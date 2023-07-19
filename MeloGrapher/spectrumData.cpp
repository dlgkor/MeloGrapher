#include"DataStruct.h"

SpectrumBlock::SpectrumBlock() {
	memset(data, 0, sizeof(double) * MAX_SPECTRUM_NUMBER);
	n_spectrum = 0;

	target_buffer = 0;
	target_sample = 0;
}

void SpectrumBlock::init() {
	memset(data, 0, sizeof(double) * MAX_SPECTRUM_NUMBER);
	n_spectrum = 0;

	target_buffer = 0;
	target_sample = 0;
}

void SpectrumBlock::fill_block(std::vector<cpx>& v) {
	memset(data, 0, sizeof(double) * MAX_SPECTRUM_NUMBER);
	n_spectrum = v.size();

	FFT(v, false);

	for (int i = 0; i < n_spectrum; i++) {
		//convert it to real size
		data[i] = sqrt(pow(v[i].real(), 2) + pow(v[i].imag(), 2));
	}
}

void SpectrumBlock::set_target(int _target_buffer, int _target_sample) {
	target_buffer = _target_buffer;
	target_sample = _target_sample;
}

int SpectrumBlock::get_target_buffer() {
	return target_buffer;
}

int SpectrumBlock::get_target_sample() {
	return target_sample;
}

int SpectrumBlock::get_total_size() const {
	return n_spectrum;
}

const double* SpectrumBlock::get_data() const {
	return data;
}

SpectrumBlock::~SpectrumBlock() {

}

void SpectrumBlock::FFT(std::vector<cpx>& v, bool inv) {
	int S = v.size();

	for (int i = 1, j = 0; i < S; i++) {
		int bit = S / 2;

		while (j >= bit) {
			j -= bit;
			bit /= 2;
		}
		j += bit;

		if (i < j) swap(v[i], v[j]);
	}

	for (int k = 1; k < S; k *= 2) {
		double angle = (inv ? M_PI / k : -M_PI / k);
		cpx w(cos(angle), sin(angle));

		for (int i = 0; i < S; i += k * 2) {
			cpx z(1, 0);

			for (int j = 0; j < k; j++) {
				cpx even = v[i + j];
				cpx odd = v[i + j + k];

				v[i + j] = even + z * odd;
				v[i + j + k] = even - z * odd;

				z *= w;
			}
		}
	}

	if (inv)
		for (int i = 0; i < S; i++) v[i] /= S;
}