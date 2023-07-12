#pragma once
#include"CommonHeader.h"
#include"WaveOut.h"

#include"BufferManager.h"

class BlockWrapper {
public:
	EncodedAudio* encoded_audio;
	WaveOut* wave_out;
	BufferManager buffer_manager;

public:
	BlockWrapper();

	void open_file(const char* filename);
	void close_file();
	void fill_audio_thread();
	void fill_spectrum_thread();
	void fill_thread();
	void play_file();
	void seek(int64_t pos);

	~BlockWrapper();
	static void CALLBACK AudioCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2) {
		if (uMsg != WOM_DONE) return;

		BlockWrapper* final_wrapper = reinterpret_cast<BlockWrapper*>(dwInstance);

		short* waveout_block_repos = nullptr;
		final_wrapper->buffer_manager.get_waveout_block(&waveout_block_repos, final_wrapper->wave_out->get_block_size());
		if (waveout_block_repos != nullptr) {
			final_wrapper->wave_out->write_wave_buffer(waveout_block_repos);
			delete waveout_block_repos;
		}
	}
};

BlockWrapper::BlockWrapper() {
	encoded_audio = nullptr;
	wave_out = nullptr;
}

void BlockWrapper::open_file(const char* filename) {
	encoded_audio = new EncodedAudio();
	encoded_audio->open(filename);
	std::cout << "oppend encoded audio" << std::endl;


	wave_out = new WaveOut(encoded_audio);
	std::cout << "allocate waveout" << std::endl;
	DWORD_PTR thisFinalWrapper = reinterpret_cast<DWORD_PTR>(this);
	wave_out->open_device((DWORD_PTR)BlockWrapper::AudioCallback, thisFinalWrapper);
	std::cout << "oppend wave_out device" << std::endl;
}

void BlockWrapper::fill_audio_thread() {
	std::cout << "fill audio loop thread" << std::endl;
	buffer_manager.fill_audio_init(encoded_audio);
	std::thread hello_thread(&BufferManager::fill_audio_loop, &buffer_manager, encoded_audio);
	hello_thread.detach();
	std::cout << "detach thread" << std::endl;
}

void BlockWrapper::fill_spectrum_thread() {
	std::cout << "fill spectrum loop thread" << std::endl;
	buffer_manager.fill_spectrum_init();
	std::thread hello_thread(&BufferManager::fill_spectrum_loop, &buffer_manager);
	hello_thread.detach();
}

void BlockWrapper::close_file() {
	buffer_manager.spectrum_off();
	buffer_manager.audio_off();

	std::chrono::milliseconds delayTime(1000); //100ms delay
	std::this_thread::sleep_for(delayTime); //delay for audio to be unprepared

	if (encoded_audio != nullptr) {
		encoded_audio->close();
		delete encoded_audio;
	}

	if (wave_out != nullptr) {
		wave_out->close_device();
		delete wave_out;
	}
}

void BlockWrapper::play_file() {
	//wait until audio_on is true
	int blocksize = wave_out->get_block_size();
	short* zero_sound = new short[blocksize];
	memset(zero_sound, 0, sizeof(short) * blocksize);

	for (int i = 0; i < wave_out->get_block_count(); i++) {
		wave_out->write_wave_buffer(zero_sound);
	}

	delete[] zero_sound;
}

void BlockWrapper::fill_thread() {
	buffer_manager.reset_block();
	fill_audio_thread();
	fill_spectrum_thread();
}

void BlockWrapper::seek(int64_t pos) {
	buffer_manager.spectrum_off();
	buffer_manager.audio_off();
	// move cursor
	encoded_audio->seek(pos);

	std::chrono::milliseconds delayTime(1000); //100ms delay
	std::this_thread::sleep_for(delayTime); //delay for audio to be unprepared

	fill_thread();
}

BlockWrapper::~BlockWrapper() {

}