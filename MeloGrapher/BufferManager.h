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
	//void fill_spectrum_block();
	//wait until block emptys by thread
	//fill empty block continuously
};

BufferManager::BufferManager() {
	//audio_block = new AudioBlock[MAX_AUDIO_BLOCK];
	//spectrum_block = new SpectrumBlock[MAX_SPECTRUM_BLOCK];
	memset(audio_block, 0, sizeof(AudioBlock*) * MAX_AUDIO_BLOCK);
	//memset(spectrum_block, 0, sizeof(SpectrumBlock*) * MAX_SPECTRUM_BLOCK);

	spectrum_block = new SpectrumBlock[MAX_SPECTRUM_BLOCK];

	audio_cursor = 0; //audio cursor for displaying and buffering
	spectrum_cursor = 0; //spectrum cursor for displaying and buffering
	waveout_cursor = 0; //waveout cursor for displaying
	audio_end_cursor = -1; //end cursor to check music end
	spectrum_end_cursor = -1;

	last_targetblock_cursor = 0;
	last_targetsample_cursor = 0;
	spectrum_cursor_gap = 32768 / 16;
	spectrum_window_size = 32768 / 4;

	audio_on = false;
	spectrum_on = false;
}

int BufferManager::get_waveout_block(short** waveout_block_repos_pointer, int waveout_block_size) {
	// extract waveout_block from target audio_block with size of waveout_block_size
	if (audio_on == false)
		return 0;

	short* waveout_block_repos_data = new short[waveout_block_size];
	memset(waveout_block_repos_data, 0, sizeof(short) * waveout_block_size);

	if (waveout_block_size + waveout_cursor > audio_block[audio_cursor]->getTotalSize()) {
		std::cout << "lock audio" << std::endl;
		std::unique_lock<std::mutex> lock(audio_mutex);

		if (audio_cursor == audio_end_cursor) {
			std::cout << "end of the audio buffer" << std::endl;
			delete[] waveout_block_repos_data;

			audio_on = false;
			return -1;
		}

		std::cout << "exceed audio cursor buffer" << std::endl;
		// exceed audio cursor buffer
		// check if target audio buffer is change		

		int audio1_size = audio_block[audio_cursor]->getTotalSize() - waveout_cursor;

		int audio2_size = waveout_block_size - audio1_size;

		short* target_audio_block = audio_block[audio_cursor]->getData(0);
		for (int sample = 0; sample < audio1_size; sample++) {
			waveout_block_repos_data[sample] = target_audio_block[waveout_cursor + sample];
		}
		waveout_cursor = 0;


		audio_cursor = (audio_cursor + 1) % MAX_AUDIO_BLOCK; // move audio_cursor

		audio2_size = min(audio2_size, audio_block[audio_cursor]->getTotalSize());
		//prevents audio2_size overs target buffer size

		target_audio_block = audio_block[audio_cursor]->getData(0);
		for (int sample = 0; sample < audio2_size; sample++) {
			waveout_block_repos_data[audio1_size + sample] = target_audio_block[waveout_cursor + sample];
		}

		waveout_cursor += audio2_size;


		audio_block_cv.notify_one(); //notify empty audio
		std::cout << "notified audio" << std::endl;
	}
	else {

		short* target_audio_block = audio_block[audio_cursor]->getData(0);
		for (int sample = 0; sample < waveout_block_size; sample++) {
			waveout_block_repos_data[sample] = target_audio_block[waveout_cursor + sample];
		}

		waveout_cursor += waveout_block_size;

		std::cout << "normal display audio" << std::endl;
	}

	*waveout_block_repos_pointer = waveout_block_repos_data;

	return 0;
}

void BufferManager::fill_audio_init(EncodedAudio* encoded_audio) {
	std::unique_lock<std::mutex> lock(audio_mutex);
	for (int block = 0; block < MAX_AUDIO_BLOCK; block++) {
		//fill 8 audio block
		if (fill_audio_block(block, encoded_audio) == -1)
			return;
	}

	audio_on = true;
	std::cout << "filled block" << std::endl;
}

void BufferManager::fill_audio_loop(EncodedAudio* encoded_audio) {

	int back_audio_cursor = 0;

	while (1) {
		//wait until notified
		//fill block before audio_cursor
		std::unique_lock<std::mutex> lock(audio_mutex);
		audio_block_cv.wait(lock);

		if (audio_on == false)
			return;

		std::cout << "audio wait released" << std::endl;
		back_audio_cursor = (audio_cursor - 1 + MAX_AUDIO_BLOCK) % MAX_AUDIO_BLOCK;

		if (fill_audio_block(back_audio_cursor, encoded_audio) == -1)
			return;

		std::cout << "filled audio block" << std::endl;
	}
}

int BufferManager::fill_audio_block(int target_cursor, EncodedAudio* encoded_audio) {
	if (audio_block[target_cursor] != nullptr)
		delete audio_block[target_cursor];

	std::cout << "filling audio block" << std::endl;

	try
	{
		audio_block[target_cursor] = encoded_audio->extract_audioblock(MIN_AUDIOBLOCK_SIZE);
	}
	catch (const char* ex) {
		std::cout << ex << std::endl;
	}

	std::cout << "extract audio block competed" << std::endl;

	if (encoded_audio->is_frame_reached_end()) {
		std::cout << "audio frame reached end" << std::endl;
		audio_end_cursor = target_cursor;
		return -1; //frame reached end
	}

	return 0;
}




int BufferManager::check_spectrum_block() {
	std::cout << "checking spectrum buffer" << std::endl;

	int audio_cursor_copy = 0;
	int waveout_cursor_copy = 0;

	{
		std::unique_lock<std::mutex> lock(audio_mutex);
		audio_cursor_copy = audio_cursor;
		waveout_cursor_copy = waveout_cursor;
	}

	int next_spectrum_cursor = (spectrum_cursor + 1) % MAX_SPECTRUM_BLOCK;

	if (next_spectrum_cursor == spectrum_end_cursor)
		return -1;

	int next_spectrum_target_buffer = spectrum_block[next_spectrum_cursor].get_target_buffer();
	int next_spectrum_sample_cursor = spectrum_block[next_spectrum_cursor].get_target_sample();

	int next_audio_cursor = (audio_cursor_copy + 1) % MAX_SPECTRUM_BLOCK;
	int before_audio_cursor = (audio_cursor_copy - 1 + MAX_SPECTRUM_BLOCK) % MAX_SPECTRUM_BLOCK;

	if (next_spectrum_target_buffer == next_audio_cursor) {
		return 0;
	}

	if (next_spectrum_target_buffer == audio_cursor_copy) {
		if (next_spectrum_sample_cursor < waveout_cursor_copy) {
			spectrum_cursor = next_spectrum_cursor;
			spectrum_block_cv.notify_one(); //notify empty audio
		}
	}

	if (next_spectrum_target_buffer == before_audio_cursor) {
		spectrum_cursor = next_spectrum_cursor;
		spectrum_block_cv.notify_one(); //notify empty audio
	}

	std::cout << "checking done" << std::endl;

	return 0;
}

int BufferManager::get_spectrum_block(SpectrumBlock* spectrum_block_repos) {
	if (spectrum_on == false)
		return  -1;

	std::unique_lock<std::mutex> lock(spectrum_mutex);

	//check spectrum block and notify
	if (check_spectrum_block() == -1) {
		spectrum_on = false;
		return -1;
	}

	*spectrum_block_repos = spectrum_block[spectrum_cursor]; //return copy of current spectrum_block instance

	return 0;
}


void BufferManager::fill_spectrum_init() {
	std::unique_lock<std::mutex> lock(spectrum_mutex);

	{
		std::unique_lock<std::mutex> lock(audio_mutex);
		for (int block = 0; block < MAX_AUDIO_BLOCK; block++) {
			if (audio_block[block] == nullptr) {
				std::cout << "one of initialized audio block is empty" << std::endl;
				spectrum_end_cursor = 0;
				return;
			}
		}
	}

	for (int block = 0; block < MAX_SPECTRUM_BLOCK; block++) {
		//initialize buffer
		if (fill_spectrum_block(block) == -1) {
			std::cout << "spectrum block reached end while initializing" << std::endl;
			return;
		}
	}

	spectrum_on = true;
	std::cout << "initialize buffer" << std::endl;
}

void BufferManager::fill_spectrum_loop() {
	int back_spectrum_cursor = 0;

	while (1) {
		std::unique_lock<std::mutex> lock(spectrum_mutex);
		spectrum_block_cv.wait(lock);

		if (spectrum_on == false)
			return;

		std::cout << "spectrum block wait released" << std::endl;
		back_spectrum_cursor = (spectrum_cursor - 1 + MAX_SPECTRUM_BLOCK) % MAX_SPECTRUM_BLOCK;
		if (fill_spectrum_block(back_spectrum_cursor) == -1) {
			std::cout << "spectrum block reached end while filling" << std::endl;
			return;
		}

		std::cout << "filled spectrum block" << std::endl;
	}
}


int BufferManager::fill_spectrum_block(int target_cursor) {
	std::cout << "lock audio mutex" << std::endl;
	std::unique_lock<std::mutex> lock(audio_mutex);

	std::vector<cpx> fft_block;
	double dMaxSample = (double)(pow(2, (sizeof(short) * 8) - 1) - 1);

	std::cout << "doing fft" << std::endl;
	last_targetsample_cursor += spectrum_cursor_gap;

	if (audio_block[last_targetblock_cursor]->getTotalSize() < last_targetsample_cursor) {
		if (last_targetblock_cursor == audio_end_cursor) {
			spectrum_end_cursor = (target_cursor - 1 + MAX_SPECTRUM_BLOCK) % MAX_SPECTRUM_BLOCK;
			return -1;
		}

		last_targetsample_cursor -= audio_block[last_targetblock_cursor]->getTotalSize();
		last_targetblock_cursor = (last_targetblock_cursor + 1) % MAX_AUDIO_BLOCK;

		if (audio_block[last_targetblock_cursor]->getTotalSize() < last_targetsample_cursor + spectrum_window_size) {
			//out of range
			//stop spectrum buffer
			std::cout << "spectrum out of range" << std::endl;
			spectrum_end_cursor = (target_cursor - 1 + MAX_SPECTRUM_BLOCK) % MAX_SPECTRUM_BLOCK;
			return -1;
		}
		else {
			short* audioblock_start = audio_block[last_targetblock_cursor]->getData(0);
			for (int sample = 0; sample < spectrum_window_size; sample++) {
				fft_block.push_back(cpx((double)audioblock_start[last_targetsample_cursor + sample] / dMaxSample, 0.0));
			}
		}

		std::cout << "last spectrum cursor is moving to next block" << std::endl;
	}
	else {
		short* audioblock_start = audio_block[last_targetblock_cursor]->getData(0);
		if (audio_block[last_targetblock_cursor]->getTotalSize() < last_targetsample_cursor + spectrum_window_size) {
			std::cout << "window oveflow" << std::endl;
			int block_size1 = audio_block[last_targetblock_cursor]->getTotalSize() - last_targetsample_cursor;
			//last target **sample** cursor

			std::cout << "block_size1: " << block_size1 << std::endl;

			for (int sample = 0; sample < block_size1; sample++) {
				fft_block.push_back(cpx((double)audioblock_start[sample + last_targetsample_cursor] / dMaxSample, 0.0));
			}

			audioblock_start = audio_block[(last_targetblock_cursor + 1) % MAX_AUDIO_BLOCK]->getData(0);
			int block_size2 = spectrum_window_size - block_size1;
			std::cout << "block_size2: " << block_size2 << std::endl;

			if (audio_block[(last_targetblock_cursor + 1) % MAX_AUDIO_BLOCK]->getTotalSize() < block_size2) {
				std::cout << "smaller then block2 size" << std::endl;
				spectrum_end_cursor = (target_cursor - 1 + MAX_SPECTRUM_BLOCK) % MAX_SPECTRUM_BLOCK;
				return -1;
			}

			for (int sample = 0; sample < block_size2; sample++) {
				fft_block.push_back(cpx((double)audioblock_start[sample] / dMaxSample, 0.0));
			}
			std::cout << "window overflow resolved" << std::endl;
		}
		else {
			std::cout << "normal fft" << std::endl;
			for (int sample = 0; sample < spectrum_window_size; sample++) {
				fft_block.push_back(cpx((double)audioblock_start[sample + last_targetsample_cursor] / dMaxSample, 0.0));
			}
		}
		std::cout << "fft within block completed" << std::endl;
	}

	std::cout << "trying to fill block" << std::endl;
	spectrum_block[target_cursor].set_target(last_targetblock_cursor, last_targetsample_cursor);
	std::cout << "set target" << std::endl;
	spectrum_block[target_cursor].fill_block(fft_block);
	std::cout << "fill block" << std::endl;
	//change last targetblock and targetsample

	//delete[] audioblock_instance;
	std::cout << "fft completed. audio block resolved" << std::endl;
	return 0;
}

void BufferManager::spectrum_off() {
	std::unique_lock<std::mutex> lock(spectrum_mutex);
	spectrum_on = false;
	spectrum_block_cv.notify_one();
}

void BufferManager::audio_off() {
	std::unique_lock<std::mutex> lock(audio_mutex);
	audio_on = false;
	audio_block_cv.notify_one();
}

void BufferManager::reset_block() {
	std::unique_lock<std::mutex> lock_1(audio_mutex);
	std::unique_lock<std::mutex> lock_2(spectrum_mutex);

	for (int i = 0; i < MAX_AUDIO_BLOCK; i++) {
		if (audio_block[i] != nullptr) {
			delete audio_block[i];
		}
	}

	delete[] spectrum_block;

	memset(audio_block, 0, sizeof(AudioBlock*) * MAX_AUDIO_BLOCK);
	spectrum_block = new SpectrumBlock[MAX_SPECTRUM_BLOCK];

	audio_cursor = 0; //audio cursor for displaying and buffering
	spectrum_cursor = 0; //spectrum cursor for displaying and buffering
	waveout_cursor = 0; //waveout cursor for displaying
	audio_end_cursor = -1; //end cursor to check music end
	spectrum_end_cursor = -1;

	last_targetblock_cursor = 0;
	last_targetsample_cursor = 0;
	spectrum_cursor_gap = 32768 / 16;
	spectrum_window_size = 32768 / 4;

	audio_on = false;
	spectrum_on = false;
}


BufferManager::~BufferManager() {
	for (int i = 0; i < MAX_AUDIO_BLOCK; i++) {
		if (audio_block[i] != nullptr) {
			delete audio_block[i];
		}
	}

	delete[] spectrum_block;
}