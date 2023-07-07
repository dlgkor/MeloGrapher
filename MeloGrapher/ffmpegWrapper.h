#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include<libavformat/avformat.h>
}

#include"audioUtils.h"

class mp3Decoder {
public:
	AVFormatContext* formatcontext;
	AVCodecContext* codecContext;
	AVPacket* packet;
	AVFrame* frame;
	int ASI;
	const AVCodec* codec;

	int channels;
	int sampleRate;
	int duration;
	int number_of_samples;

	double dMaxSample; //그 값을 double로 형변환

	double clip(double dSample, double dMax)
	{
		if (dSample >= 0.0)
			return fmin(dSample, dMax);
		else
			return fmax(dSample, -dMax);
	}
public:
	mp3Decoder(const char* filename) {
		avformat_network_init();
		formatcontext = nullptr;
		codecContext = nullptr;
		packet = nullptr;
		frame = nullptr;

		dMaxSample = (double)(pow(2, (sizeof(short) * 8) - 1) - 1);

		if (avformat_open_input(&formatcontext, filename, NULL, NULL) != 0) {
			//std::cout << "failed to open file" << std::endl;
			return;
		}

		//get all stream information from file
		if (avformat_find_stream_info(formatcontext, NULL) < 0) {
			//std::cout << "failed to receive input stream information" << std::endl;
			return;
		}

		av_dump_format(formatcontext, 0, filename, 0); //print format information

		ASI = av_find_best_stream(formatcontext, AVMEDIA_TYPE_AUDIO, -1, NULL, NULL, 0); //find best audio stream key value
		codecContext = avcodec_alloc_context3(nullptr);
		if (codecContext == nullptr) {
			//std::cout << "cannot allocate codec contest" << std::endl;
			return;
		}
		avcodec_parameters_to_context(codecContext, formatcontext->streams[ASI]->codecpar);

		codec = avcodec_find_decoder(codecContext->codec_id);
		//recent version ffmpeg changed avcodec to be declared constant

		if (codec == nullptr) {
			//std::cout << "cannot find codec" << std::endl;
			return;
		}
		if (avcodec_open2(codecContext, codec, nullptr) < 0) {
			//std::cout << "cannot open codec" << std::endl;
			return;
		}

		packet = av_packet_alloc();
		frame = av_frame_alloc();

		channels = codecContext->ch_layout.nb_channels; //number of channels
		sampleRate = codecContext->sample_rate; //sample per sec
		duration = formatcontext->duration / AV_TIME_BASE; //duration[second]
		number_of_samples = sampleRate * (duration + 1);
	}

	int get_audiodata(AVFrame* frame, AudioData* audio, int audio_cursor) {
		uint8_t** data = frame->extended_data; //sample data(real audio data) pointer
		const int samples = frame->nb_samples; //sample per frame. generally 1152

		const AVSampleFormat sampleFormat = static_cast<AVSampleFormat>(frame->format);

		//planar audio uses extended_data
		for (int channel = 0; channel < channels; ++channel) {
			if (sampleFormat == AV_SAMPLE_FMT_FLTP) {
				for (int sample = 0; sample < samples; ++sample) {
					const float amplitude = *reinterpret_cast<const float*>(data[channel] + sample * av_get_bytes_per_sample(sampleFormat));
					audio->data[channel][audio_cursor + sample] = (short)(clip((double)amplitude, 1.0) * dMaxSample);
				}
			}
			else if (sampleFormat == AV_SAMPLE_FMT_DBLP) {
				for (int sample = 0; sample < samples; ++sample) {
					const double amplitude = *reinterpret_cast<const double*>(data[channel] + sample * av_get_bytes_per_sample(sampleFormat));
					audio->data[channel][audio_cursor + sample] = (short)(clip(amplitude, 1.0) * dMaxSample);
				}
			}
			else if (sampleFormat == AV_SAMPLE_FMT_S16P) {
				for (int sample = 0; sample < samples; ++sample) {
					const short amplitude = *reinterpret_cast<const short*>(data[channel] + sample * av_get_bytes_per_sample(sampleFormat));
					audio->data[channel][audio_cursor + sample] = amplitude;
				}
			}
			else {
				//unsupported format
				return -1;
			}
		}

		return 0;
	}

	AudioData* decode_mp3(int n_sample) {
		const int MAX_EXTRA_SIZE = 10000;
		AudioData* audio = new AudioData(channels, n_sample + MAX_EXTRA_SIZE);
		int audio_cursor = 0;

		while (audio_cursor < n_sample) {
			if (av_read_frame(formatcontext, packet) < 0) {
				delete audio;
				av_frame_unref(frame);
				return nullptr;
			}

			if (packet->stream_index == ASI) {
				if (avcodec_send_packet(codecContext, packet)) {
					//std::cout << "avcodec send packet failed" << std::endl;
				}
				while (avcodec_receive_frame(codecContext, frame) >= 0) {
					get_audiodata(frame, audio, audio_cursor);
					
					audio_cursor += frame->nb_samples;
				}
			}

			av_packet_unref(packet);
		}

		av_frame_unref(frame);

		audio->n_samples = audio_cursor; //update number of samples in audiodata

		return audio;
	}

	AudioData* decode_mp3_all() {
		AudioData* audio = new AudioData(channels, number_of_samples);
		int audio_cursor = 0;

		while (av_read_frame(formatcontext, packet) >= 0) {
			if (packet->stream_index == ASI) {
				if (avcodec_send_packet(codecContext, packet)) {
					//std::cout << "avcodec send packet failed" << std::endl;
				}
				while (avcodec_receive_frame(codecContext, frame) >= 0) {
					get_audiodata(frame, audio, audio_cursor);

					audio_cursor += frame->nb_samples;
				}
			}

			av_packet_unref(packet);
		}

		av_frame_unref(frame);

		audio->n_samples = audio_cursor;

		return audio;
	}

	~mp3Decoder() {
		av_frame_free(&frame);
		av_packet_free(&packet);

		avcodec_close(codecContext);
		avformat_close_input(&formatcontext);

		avformat_network_deinit();
	}
};