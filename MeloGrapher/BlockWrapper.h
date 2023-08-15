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
	void set_buffer_manager(melo::SpectrumOption spectrum_option);
	//call from windowwrapper set_main_data

	~BlockWrapper();
	static void CALLBACK AudioCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);
};