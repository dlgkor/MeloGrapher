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