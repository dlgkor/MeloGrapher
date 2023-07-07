#pragma once
#pragma comment(lib, "winmm.lib")
#include"common.h"

class W_Sound {
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
	int m_is_start;
public:
	W_Sound() {
		m_nBlockSamples = 512; //���� �ϳ��� ���ü�
		m_nSampleRate = 44100; //1�ʿ� ����Ǵ� ���ü�
		m_nBlockCount = 8; //������ ����
		m_nBlockCurrent = 0;

		waveFormat.wFormatTag = WAVE_FORMAT_PCM; //���� ������� ���� Ÿ��(���� �˰����)
		waveFormat.nSamplesPerSec = m_nSampleRate; //���÷���Ʈ
		waveFormat.wBitsPerSample = sizeof(short) * 8; //�Ҹ� ������ short������ ����Ǿ��ٴ� �� ������ ũ�⸦ ���� �˷��ش� //�� ���� ������ 8bit �ƴϸ� 16bit�̴�
		waveFormat.nChannels = 1; //�Ҹ��� ����Ǵ� ����� ����, �� ä�ΰ����̴�
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; //������ ũ��(����Ʈ ����) * ä�μ�
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; //1�ʿ� �ʿ��� ����Ʈ��
		waveFormat.cbSize = 0;

		m_is_start = 0;
	}
	int OpenDevice(void* notify_function) {
		if (waveOutOpen(&m_hwDevice, NULL, &waveFormat, (DWORD_PTR)notify_function, (DWORD_PTR)0, (DWORD)(CALLBACK_FUNCTION)) != S_OK)
			return 0;
		m_pWaveHeader = new WAVEHDR[m_nBlockCount]; //���� ������ŭ wavehdr �޸� �Ҵ�
		ZeroMemory(m_pWaveHeader, sizeof(WAVEHDR) * m_nBlockCount); //�޸𸮸� ��� ����

		buffer = new short[m_nBlockCount * m_nBlockSamples]; //�� ������ ũ���� ���� ����*���� ���� ��ŭ �޸𸮸� �Ҵ��Ѵ�
		ZeroMemory(buffer, sizeof(short) * m_nBlockCount * m_nBlockSamples); //���������� �޸𸮸� ��� ����

		for (unsigned int n = 0; n < m_nBlockCount; n++) {
			m_pWaveHeader[n].lpData = (LPSTR)(buffer + (n * m_nBlockSamples)); //lpdata�� ���۸� �����Ѵ�
			m_pWaveHeader[n].dwBufferLength = m_nBlockSamples * sizeof(short); //lpdata�� ����� ������ ũ�⸦ �˷��ش�
		}
		return 1;
	}
	void StartDevice() {
		if (m_is_start == 0)
			m_is_start = 1;
	}
	void WriteWaveBuffer(short* PrintBuffer) {
		int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples; //ó���ϴ� ������ ������ 0, 512, 1024 ...
		short nNewSample = 0; //���� ��������

		//wavehdr ���۸� �غ��Ų��
		if (m_pWaveHeader[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

		for (unsigned int n = 0; n < m_nBlockSamples; n++)
		{
			nNewSample = PrintBuffer[n];
			buffer[nCurrentBlock + n] = nNewSample;
			//������ ���ð�����ŭ �о����

			ct_Sample++;
			//�����ð� ������Ŵ
		}

		waveOutPrepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
		waveOutWrite(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

		m_nBlockCurrent++; //�������۷� �̵�
		m_nBlockCurrent %= m_nBlockCount; //������ ���� �ȿ��� ���� ���۸� ������ִ� ������ ����
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