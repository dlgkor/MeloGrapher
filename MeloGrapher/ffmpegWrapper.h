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