#pragma once

#include"CommonHeader.h"
#include"FFmpegWrapper.h"

class WaveOut {
private:
	HWAVEOUT m_hwDevice;
	short* buffer; //wavhdr.lpdata에 연결할 short 버퍼(2바이트)
	WAVEHDR* m_pWaveHeader;	//소리 데이터가 담겨있다

	unsigned int m_nBlockSamples; //버퍼 하나당 샘플수
	unsigned int m_nSampleRate; //1초에 재생되는 샘플수
	unsigned int m_nBlockCount; //버퍼의 개수
	unsigned int m_nBlockCurrent; //현재 출력중인 버퍼

	WAVEFORMATEX waveFormat; //파형 오디오 데이터의 포맷을 결정한다

	unsigned int ct_Sample;
public:
	WaveOut(EncodedAudio* const encoded_audio);
	int open_device(DWORD_PTR notify_function, DWORD_PTR dwInstance);
	void close_device();
	void write_wave_buffer(short* wave_buffer);
	int get_block_size();
	int get_block_count();
	~WaveOut();
};