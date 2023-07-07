#pragma once
#include"common.h"

void FFT(std::vector<cpx>& v, bool inv) {
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