#pragma once
#pragma comment(lib, "winmm.lib")
#include"common.h"

class W_Sound {
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
	int m_is_start;
public:
	W_Sound() {
		m_nBlockSamples = 512; //버퍼 하나당 샘플수
		m_nSampleRate = 44100; //1초에 재생되는 샘플수
		m_nBlockCount = 8; //버퍼의 개수
		m_nBlockCurrent = 0;

		waveFormat.wFormatTag = WAVE_FORMAT_PCM; //파형 오디오의 포맷 타입(압축 알고리즘용)
		waveFormat.nSamplesPerSec = m_nSampleRate; //샘플레이트
		waveFormat.wBitsPerSample = sizeof(short) * 8; //소리 샘플이 short형으로 저장되었다는 걸 데이터 크기를 통해 알려준다 //이 값은 무조건 8bit 아니면 16bit이다
		waveFormat.nChannels = 1; //소리가 재생되는 기기의 개수, 즉 채널개수이다
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; //샘플의 크기(바이트 단위) * 채널수
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; //1초에 필요한 바이트수
		waveFormat.cbSize = 0;

		m_is_start = 0;
	}
	int OpenDevice(void* notify_function) {
		if (waveOutOpen(&m_hwDevice, NULL, &waveFormat, (DWORD_PTR)notify_function, (DWORD_PTR)0, (DWORD)(CALLBACK_FUNCTION)) != S_OK)
			return 0;
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
	void StartDevice() {
		if (m_is_start == 0)
			m_is_start = 1;
	}
	void WriteWaveBuffer(short* PrintBuffer) {
		int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples; //처리하는 버퍼의 시작점 0, 512, 1024 ...
		short nNewSample = 0; //샘플 단위버퍼

		//wavehdr 버퍼를 준비시킨다
		if (m_pWaveHeader[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

		for (unsigned int n = 0; n < m_nBlockSamples; n++)
		{
			nNewSample = PrintBuffer[n];
			buffer[nCurrentBlock + n] = nNewSample;
			//버퍼의 샘플개수만큼 읽어들임

			ct_Sample++;
			//측정시간 증가시킴
		}

		waveOutPrepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
		waveOutWrite(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

		m_nBlockCurrent++; //다음버퍼로 이동
		m_nBlockCurrent %= m_nBlockCount; //버퍼의 개수 안에서 현재 버퍼를 계산해주는 나머지 연산
	}
	int StopDevice() {
		if (m_is_start) {
			MMRESULT errorcode = waveOutPause((HWAVEOUT)m_hwDevice);
			if (errorcode == MMSYSERR_NOERROR) {
				waveOutReset((HWAVEOUT)m_hwDevice);

				for (int i = 0; i < m_nBlockCount; i++) {
					waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[i], sizeof(WAVEHDR));
				}
			}
			m_is_start = 0;
			return 1;
		}
		return 0;
	}
	void CloseDeivce() {
		waveOutClose(m_hwDevice);
	}
	void ResetCurrentSampleNum() {
		ct_Sample = 0;
	}
	unsigned int GetCurrentSampleNum() {
		return ct_Sample;
	}
	unsigned int GetBlockSamples() {
		return m_nBlockSamples;
	}
	unsigned int GetBlockN() {
		return m_nBlockCount;
	}
	~W_Sound() {
		delete[] m_pWaveHeader;
		delete[] buffer;
	}
};