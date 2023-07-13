#pragma once

#include"CommonHeader.h"
#include"FFmpegWrapper.h"

class WaveOut {
private:
	HWAVEOUT m_hwDevice;
	short* buffer; //wavhdr.lpdata�� ������ short ����(2����Ʈ)
	WAVEHDR* m_pWaveHeader;	//�Ҹ� �����Ͱ� ����ִ�

	unsigned int m_nBlockSamples; //���� �ϳ��� ���ü�
	unsigned int m_nSampleRate; //1�ʿ� ����Ǵ� ���ü�
	unsigned int m_nBlockCount; //������ ����
	unsigned int m_nBlockCurrent; //���� ������� ����

	WAVEFORMATEX waveFormat; //���� ����� �������� ������ �����Ѵ�

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