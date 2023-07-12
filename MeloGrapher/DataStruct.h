#pragma once
#include"CommonHeader.h"

#define MAX_AUDIO_CHANNELS 3
#define MAX_AUDIO_SAMPLES 441000
#define MAX_SPECTRUM_NUMBER 441000

class AudioBlock {
private:
	short data[MAX_AUDIO_CHANNELS][MAX_AUDIO_SAMPLES]; //real sample data
	int channels; //number of channels

	//int max_samples; //maximum sample number
	int total_samples; //real sample number
public:
	AudioBlock();
	void set_channel_size(int channel_size);
	int getTotalSize();
	short* getData(int channel); //get start pointer of certain channel
	void addSize(int delta); //change total_samples value by adding delta
	~AudioBlock();
};

AudioBlock::AudioBlock() {
	total_samples = 0;

	for (int channel = 0; channel < MAX_AUDIO_CHANNELS; channel++) {
		memset(data[channel], 0, sizeof(short) * MAX_AUDIO_SAMPLES);
	}
}

void AudioBlock::set_channel_size(int channel_size) {
	if (channel_size >= MAX_AUDIO_CHANNELS) {
		throw "audioblock channel is larger than MAX_AUDIO_CHANNELS";
	}

	channels = channel_size;
}

int AudioBlock::getTotalSize() {
	return total_samples;
}

short* AudioBlock::getData(int channel) {
	return data[channel];
}

void AudioBlock::addSize(int delta) {
	if (delta <= 0) {
		throw "add 0 or less size to audioblock";
	}
	total_samples += delta;
}

AudioBlock::~AudioBlock() {

}












class SpectrumBlock {
private:
	double data[MAX_SPECTRUM_NUMBER]; //real spectrum data
	int n_spectrum; //number of spectrumn data

	int target_buffer; //target audio buffer
	int target_sample; //target sample in target audio buffer
	//target sample means the left side of fft window
public:
	SpectrumBlock();
	void init();
	void fill_block(std::vector<cpx>& v);
	void set_target(int _target_buffer, int _target_sample);
	int get_target_buffer();
	int get_target_sample();
	int get_total_size() const;
	const double* get_data() const;
	~SpectrumBlock();
private:
	void FFT(std::vector<cpx>& v, bool inv);
};

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