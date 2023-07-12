#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include<libavformat/avformat.h>
}

#include"CommonHeader.h"
#include"DataStruct.h"

class EncodedAudio
{
private:
	AVFormatContext* formatContext;
	AVCodecContext* codecContext;
	AVPacket* packet;
	AVFrame* frame;

	int audio_stream_index;

	int channels;
	int sampleRate;

	int64_t timeBaseNum;
	int64_t timeBaseDen;
	int64_t duration;
	int64_t start_time;

	//int duration_sec;


	double dMaxSample;

	bool file_opened;
	bool frame_reached_end; // show if frame is remaning
public:
	EncodedAudio();
	void open(const char* filename);
	void close();
	AudioBlock* extract_audioblock(int min_blockSize);
	WAVEFORMATEX get_base_format();
	bool is_frame_reached_end();

	int seek(int64_t pos);
	int seekTo(int64_t offset);
	
	int get_sample_rate() const;
	int64_t get_duration() const;

	~EncodedAudio();
private:
	double clip(double dSample, double dMax);
	int read_frame(AudioBlock* target_block); //read encoded frame and decode it to target_audioblock
	void read_pcm(AudioBlock* target_block, AVFrame* target_frame);
};

EncodedAudio::EncodedAudio()
{
	formatContext = nullptr;
	codecContext = nullptr;
	packet = nullptr;
	frame = nullptr;

	audio_stream_index = 0;

	channels = 0;
	sampleRate = 0;

	timeBaseNum = 0;
	timeBaseDen = 0;
	duration = 0;

	start_time = 0;

	dMaxSample = (double)(pow(2, (sizeof(short) * 8) - 1) - 1);

	file_opened = false;
	frame_reached_end = false;

	avformat_network_init();
}

void EncodedAudio::open(const char* filename) {
	if (avformat_open_input(&formatContext, filename, NULL, NULL) != 0) {
		throw "failed to open file";
	}

	//get all stream information from file
	if (avformat_find_stream_info(formatContext, NULL) < 0) {
		throw "failed to receive input stream information";
	}

	audio_stream_index = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, NULL, NULL, 0); //find best audio stream key value
	codecContext = avcodec_alloc_context3(nullptr);
	if (codecContext == nullptr) {
		throw "failed to allocate codec context";
	}
	avcodec_parameters_to_context(codecContext, formatContext->streams[audio_stream_index]->codecpar);

	channels = codecContext->ch_layout.nb_channels; //number of channels
	sampleRate = codecContext->sample_rate; //sample per sec

	timeBaseNum = formatContext->streams[audio_stream_index]->time_base.num;
	timeBaseDen = formatContext->streams[audio_stream_index]->time_base.den;
	duration = formatContext->duration * timeBaseNum / timeBaseDen;
	start_time = formatContext->start_time * timeBaseNum / timeBaseDen;

	const AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);

	if (codec == nullptr) {
		throw "failed to find codec";
	}
	if (avcodec_open2(codecContext, codec, nullptr) < 0) {
		throw "failed to initialize codeccontext with codec";
	}

	packet = av_packet_alloc();
	frame = av_frame_alloc();

	file_opened = true;
	frame_reached_end = false;
}

void EncodedAudio::close() {

	if (!file_opened)
		return;

	av_frame_free(&frame);
	av_packet_free(&packet);

	avcodec_close(codecContext);
	avformat_close_input(&formatContext);

	file_opened = false;
}

WAVEFORMATEX EncodedAudio::get_base_format() {
	//fill waveformat

	WAVEFORMATEX waveFormat;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.wBitsPerSample = sizeof(short) * 8;
	waveFormat.nSamplesPerSec = sampleRate;
	waveFormat.nChannels = 1;
	//waveFormat.nChannels = channels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	return waveFormat;
}

int EncodedAudio::seek(int64_t pos) {
	//check if it is on boundary
	avcodec_flush_buffers(codecContext);
	av_seek_frame(formatContext, audio_stream_index, start_time + pos, AVSEEK_FLAG_BACKWARD);
	return 0;
}

int EncodedAudio::seekTo(int64_t offset) {
	//move by offset from current location
	return 0;
}



int64_t EncodedAudio::get_duration() const {
	return duration;
}

AudioBlock* EncodedAudio::extract_audioblock(int min_blockSize) {
	const AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
	AudioBlock* audio_block = nullptr;

	audio_block = new AudioBlock();
	audio_block->set_channel_size(channels);

	while (audio_block->getTotalSize() < min_blockSize) {
		int ref = read_frame(audio_block);

		if (ref == -1) {
			frame_reached_end = true;
			return audio_block;
		}
	}

	av_frame_unref(frame);

	return audio_block;
}

int EncodedAudio::read_frame(AudioBlock* target_block) {
	if (av_read_frame(formatContext, packet) < 0) {
		//end of frame
		return -1;
	}

	if (packet->stream_index == audio_stream_index) {
		if (avcodec_send_packet(codecContext, packet)) {
			throw "failed to send packet for decode";
		}
		while (avcodec_receive_frame(codecContext, frame) >= 0) {
			try
			{
				read_pcm(target_block, frame);
			}
			catch (const char* ex)
			{
				throw;
			}
			//add pcm data to target audioblock
		}

		av_packet_unref(packet);
	}



	return 0;
}

void EncodedAudio::read_pcm(AudioBlock* target_block, AVFrame* target_frame) {
	uint8_t** data_planar = target_frame->extended_data; //sample data pointer
	uint8_t** data_non_planar = target_frame->data;

	const int samples = target_frame->nb_samples; //sample per frame

	const AVSampleFormat sampleFormat = static_cast<AVSampleFormat>(target_frame->format);

	int av_bytes_per_sample = av_get_bytes_per_sample(sampleFormat);

	//planar_audio
	short* audio_pointer = nullptr;
	if (sampleFormat == AV_SAMPLE_FMT_FLTP) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const float amplitude = *reinterpret_cast<const float*>(data_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = (short)(clip((double)amplitude, 1.0) * dMaxSample);
			}
		}
	}
	else if (sampleFormat == AV_SAMPLE_FMT_DBLP) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const double amplitude = *reinterpret_cast<const double*>(data_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = (short)(clip(amplitude, 1.0) * dMaxSample);
			}
		}
	}
	else if (sampleFormat == AV_SAMPLE_FMT_S16P) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const short amplitude = *reinterpret_cast<const short*>(data_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = amplitude;
			}
		}
	}
	//non planar audio
	/*
	else if (sampleFormat == AV_SAMPLE_FMT_FLT) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const float amplitude = *reinterpret_cast<const float*>(data_non_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = (short)(clip((double)amplitude, 1.0) * dMaxSample);
			}
		}
	}
	else if (sampleFormat == AV_SAMPLE_FMT_DBL) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const double amplitude = *reinterpret_cast<const double*>(data_non_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = (short)(clip(amplitude, 1.0) * dMaxSample);
			}
		}
	}
	else if (sampleFormat == AV_SAMPLE_FMT_S16) {
		for (int channel = 0; channel < channels; channel++) {
			audio_pointer = target_block->getData(channel);
			for (int sample = 0; sample < samples; sample++) {
				const short amplitude = *reinterpret_cast<const short*>(data_non_planar[channel] + sample * av_bytes_per_sample);
				audio_pointer[target_block->getTotalSize() + sample] = amplitude;
			}
		}
	}
	*/

	else {
		//unsupported format
		//|| sampleFormat == AV_SAMPLE_FMT_FLT
		//|| sampleFormat == AV_SAMPLE_FMT_DBL
		//|| sampleFormat == AV_SAMPLE_FMT_S16

		std::cout << sampleFormat << std::endl;
		throw "unsupported audio sample format";
	}

	try
	{
		target_block->addSize(samples);
	}
	catch (const char* ex)
	{
		throw;
	}
}

bool EncodedAudio::is_frame_reached_end() {
	return frame_reached_end;
}

int EncodedAudio::get_sample_rate() const{
	return sampleRate;
}

double EncodedAudio::clip(double dSample, double dMax)
{
	if (dSample >= 0.0)
		return fmin(dSample, dMax);
	else
		return fmax(dSample, -dMax);
}

EncodedAudio::~EncodedAudio()
{
	if (file_opened)
		close();

	avformat_network_deinit();
}