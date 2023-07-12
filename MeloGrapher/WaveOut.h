#pragma once
#include"CommonHeader.h"
#include"FFmpegWrapper.h"

#pragma comment(lib, "winmm.lib")

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

WaveOut::WaveOut(EncodedAudio* const encoded_audio) {
	waveFormat = encoded_audio->get_base_format();

	m_nBlockSamples = 512; //버퍼 하나당 샘플수
	m_nSampleRate = waveFormat.nSamplesPerSec; //1초에 재생되는 샘플수
	m_nBlockCount = 8; //버퍼의 개수
	m_nBlockCurrent = 0;
}

int WaveOut::open_device(DWORD_PTR notify_function, DWORD_PTR dwInstance) {
	if (waveOutOpen(&m_hwDevice, NULL, &waveFormat, notify_function, dwInstance, (DWORD)(CALLBACK_FUNCTION)) != S_OK)
		return 0;
	//dwInstance

	std::cout << "waveoutopen" << std::endl;

	m_pWaveHeader = new WAVEHDR[m_nBlockCount]; //버퍼 개수만큼 wavehdr 메모리 할당
	ZeroMemory(m_pWaveHeader, sizeof(WAVEHDR) * m_nBlockCount); //메모리를 모두 비운다

	buffer = new short[m_nBlockCount * m_nBlockSamples]; //총 버퍼의 크기인 버퍼 개수*버퍼 길이 만큼 메모리를 할당한다
	ZeroMemory(buffer, sizeof(short) * m_nBlockCount * m_nBlockSamples); //마찬가지로 메모리를 모두 비운다

	for (unsigned int n = 0; n < m_nBlockCount; n++) {
		m_pWaveHeader[n].lpData = (LPSTR)(buffer + (n * m_nBlockSamples)); //lpdata와 버퍼를 연결한다
		m_pWaveHeader[n].dwBufferLength = m_nBlockSamples * sizeof(short); //lpdata에 연결된 버퍼의 크기를 알려준다
	}
	return 1;
}

void WaveOut::write_wave_buffer(short* wave_buffer) {
	int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples; //처리하는 버퍼의 시작점 0, 512, 1024 ...
	short nNewSample = 0; //샘플 단위버퍼

	//wavehdr 버퍼를 준비시킨다
	if (m_pWaveHeader[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

	for (unsigned int n = 0; n < m_nBlockSamples; n++)
	{
		nNewSample = wave_buffer[n];
		buffer[nCurrentBlock + n] = nNewSample;
		//버퍼의 샘플개수만큼 읽어들임

		ct_Sample++;
		//측정시간 증가시킴
	}

	waveOutPrepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
	waveOutWrite(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
	waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

	m_nBlockCurrent++; //다음버퍼로 이동
	m_nBlockCurrent %= m_nBlockCount; //버퍼의 개수 안에서 현재 버퍼를 계산해주는 나머지 연산
}

int WaveOut::get_block_size() {
	return m_nBlockSamples;
}

int WaveOut::get_block_count() {
	return m_nBlockCount;
}

void WaveOut::close_device() {
	delete[] m_pWaveHeader;
	delete[] buffer;
	waveOutReset((HWAVEOUT)m_hwDevice);
	waveOutClose(m_hwDevice);
}

WaveOut::~WaveOut() {

}