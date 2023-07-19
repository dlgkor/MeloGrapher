#pragma once

#include"CommonHeader.h"
#include"FFmpegWrapper.h"

//thread wait for buffer to be empty
//fill buffer
//display if buffer is not empty

#define MAX_AUDIO_BLOCK 3
#define MAX_SPECTRUM_BLOCK 3

#define MIN_AUDIOBLOCK_SIZE 132300
//size of minimum audio block sample size

class BufferManager {
private:
	AudioBlock* audio_block[MAX_AUDIO_BLOCK];

	SpectrumBlock* spectrum_block;

	int audio_cursor; //points current audio block
	int spectrum_cursor; //points current spectrum block

	int waveout_cursor; //points current sample in block pointed by audio_cursor
	int audio_end_cursor;

	std::mutex audio_mutex;
	std::condition_variable audio_block_cv; //lock to protect variales like audio_cursor, waveout_cursor

	std::mutex spectrum_mutex;
	std::condition_variable spectrum_block_cv;

	int last_targetblock_cursor; //target buffer where latest buffer fill happend
	int last_targetsample_cursor; //target sample where latest buffer fill happend

	int spectrum_cursor_gap;
	int spectrum_window_size;

	int spectrum_end_cursor;

	bool audio_on;
	bool spectrum_on;
public:
	BufferManager();

	int get_waveout_block(short** waveout_block_repos, int waveout_block_size);
	int get_spectrum_block(SpectrumBlock* spectrum_block_repos);
	int check_spectrum_block();

	void fill_audio_init(EncodedAudio* encoded_audio);
	void fill_spectrum_init();
	void fill_audio_loop(EncodedAudio* encoded_audio);
	void fill_spectrum_loop();

	void spectrum_off();
	void audio_off();
	void reset_block();

	~BufferManager();
private:
	int fill_audio_block(int target_cursor, EncodedAudio* encoded_audio);
	int fill_spectrum_block(int target_cursor);
};