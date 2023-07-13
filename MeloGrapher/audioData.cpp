#include"DataStruct.h"

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