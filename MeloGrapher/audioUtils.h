#pragma once
#include"common.h"

class AudioData {
public:
	short** data; //row : channel, column : sample
	//int cur;
	int channels;
	int n_samples;

public:
	AudioData() {
		data = nullptr;
		//cur = 0; //cursor
		channels = 0;
		n_samples = 0;
	}

	AudioData(const AudioData& other) {
		//deep copy

		this->data = nullptr;
		//this->cur = 0;

		this->channels = other.channels;
		this->n_samples = other.n_samples;

		this->data = new short* [channels];
		for (int i = 0; i < channels; i++) {
			this->data[i] = new short[n_samples];
		}

		for (int channel = 0; channel < this->channels; channel++) {
			for (int sample = 0; sample < this->n_samples; sample++) {
				this->data[channel][sample] = other.data[channel][sample];
			}
		}
	}

	void Init(int _channels, int _n_samples) {
		channels = _channels;
		n_samples = _n_samples;
		if (data != nullptr) {
			for (int i = 0; i < channels; i++) {
				delete[] data[i];
			}
			delete[] data;

			data = nullptr;
		}

		data = new short* [channels];
		//cur = 0;
		for (int i = 0; i < channels; i++) {
			data[i] = new short[n_samples];
		}
	}

	AudioData(int _channels, int _n_samples)
		: channels(_channels), n_samples(_n_samples) {
		data = nullptr;
		//cur = 0; //cursor

		//allocate 2d array
		data = new short* [channels];
		for (int i = 0; i < channels; i++) {
			data[i] = new short[n_samples];
		}
	}

	void ClearData() {
		for (int channel = 0; channel < this->channels; channel++) {
			for (int sample = 0; sample < this->n_samples; sample++) {
				data[channel][sample] = 0;
			}
		}
	}

	~AudioData() {
		if (data == nullptr)
			return;

		for (int i = 0; i < channels; i++) {
			delete[] data[i];
		}
		delete[] data;

		data = nullptr;
	}
};



class audioCapacitor {
private:
	AudioData* audio;
	int cur;
	int remain;

	int channels;
	int n_maxSample;
public:
	audioCapacitor() {
		cur = 0;
		remain = 0;
		audio = nullptr;
		channels = 0;
		n_maxSample = 0;
	}
	void InitAudio(int _channels, int _n_maxSample) {
		if (audio != nullptr)
			delete audio;

		channels = _channels;
		n_maxSample = _n_maxSample;

		audio = new AudioData(channels, n_maxSample);
	}
	int Push(AudioData start, int d_size) {
		if (remain + d_size > audio->n_samples) {
			//push error. over capacity limit
			return -1;
		}

		for (int channel = 0; channel < channels; channel++) {
			for (int i = 0; i < d_size; i++) {
				audio->data[channel][(cur + remain + i) % n_maxSample] = start.data[channel][i];
			}
		}
		remain += d_size;
	}
	int Pull(AudioData* p_Data, int d_size, int read_cursor) {
		int pull_size = min(remain, d_size);

		for (int channel = 0; channel < channels; channel++) {
			for (int sample = 0; sample < pull_size; sample++) {
				p_Data->data[channel][read_cursor + sample] = audio->data[channel][(cur + sample) % n_maxSample];
			}
		}
		//p_Data->cur += pull_size;
		cur = (cur + pull_size) % n_maxSample;
		remain -= pull_size;

		return pull_size;
	}
	~audioCapacitor() {
		if (audio != nullptr) {
			delete audio;
		}
	}
};