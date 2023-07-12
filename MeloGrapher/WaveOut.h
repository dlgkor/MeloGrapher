#pragma once
#include"CommonHeader.h"
#include"FFmpegWrapper.h"

#pragma comment(lib, "winmm.lib")

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

WaveOut::WaveOut(EncodedAudio* const encoded_audio) {
	waveFormat = encoded_audio->get_base_format();

	m_nBlockSamples = 512; //���� �ϳ��� ���ü�
	m_nSampleRate = waveFormat.nSamplesPerSec; //1�ʿ� ����Ǵ� ���ü�
	m_nBlockCount = 8; //������ ����
	m_nBlockCurrent = 0;
}

int WaveOut::open_device(DWORD_PTR notify_function, DWORD_PTR dwInstance) {
	if (waveOutOpen(&m_hwDevice, NULL, &waveFormat, notify_function, dwInstance, (DWORD)(CALLBACK_FUNCTION)) != S_OK)
		return 0;
	//dwInstance

	std::cout << "waveoutopen" << std::endl;

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

void WaveOut::write_wave_buffer(short* wave_buffer) {
	int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples; //ó���ϴ� ������ ������ 0, 512, 1024 ...
	short nNewSample = 0; //���� ��������

	//wavehdr ���۸� �غ��Ų��
	if (m_pWaveHeader[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

	for (unsigned int n = 0; n < m_nBlockSamples; n++)
	{
		nNewSample = wave_buffer[n];
		buffer[nCurrentBlock + n] = nNewSample;
		//������ ���ð�����ŭ �о����

		ct_Sample++;
		//�����ð� ������Ŵ
	}

	waveOutPrepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
	waveOutWrite(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
	waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

	m_nBlockCurrent++; //�������۷� �̵�
	m_nBlockCurrent %= m_nBlockCount; //������ ���� �ȿ��� ���� ���۸� ������ִ� ������ ����
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