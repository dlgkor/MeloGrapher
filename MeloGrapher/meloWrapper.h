#pragma once
#include"common.h"
#include"meloGrapher.h"
#include"noiseMaker.h"

namespace melo {
	class Player {
	private:
		mp3Decoder* m_decoder;
		BufferWrapper* m_buffer;
		W_Sound* w_sound;

		bool music_start;
	public:
		Player(HWND hWnd) {
			m_decoder = nullptr;
			m_buffer = nullptr;
			w_sound = new W_Sound();

			w_sound->OpenDevice(hWnd);

			music_start = false;
		}
		void setSpecturmOption() {
			SpectrumOption spectrum_option;

			spectrum_option.s_gap = 32768 / 2;
			spectrum_option.s_window = 32768 / 4;
			spectrum_option.s_window_half = spectrum_option.s_window / 2;
			spectrum_option.base_frequency = (double)m_decoder->sampleRate / (double)spectrum_option.s_window;

			spectrum_option.max_out_frequency = 1000;
			spectrum_option.min_out_frequency = 10;
			spectrum_option.max_height = 400;
			spectrum_option.n_graph = 100; 
			spectrum_option.r_center = { 0, 0 };
			spectrum_option.radius = 200;

			m_buffer->set_spectrum_option(spectrum_option);
		}

		void readAudio(const char* filename) {
			if (m_decoder != nullptr) {
				delete m_decoder;
			}
			if (m_buffer != nullptr) {
				delete m_buffer;
			}

			m_decoder = new mp3Decoder(filename);

			m_buffer = new melo::BufferWrapper();
			m_buffer->set_audio_option(m_decoder->channels, 44100);
		}
		void Start() {
			w_sound->StartDevice();

			int block_sample = w_sound->GetBlockSamples();
			short* buffer = new short[block_sample];
			memset(buffer, 0, sizeof(short) * block_sample);

			for (int i = 0; i < w_sound->GetBlockN(); i++) {
				w_sound->WriteWaveBuffer(buffer);
			}
			delete[] buffer;

			std::thread f_thread(&BufferWrapper::fill_buffer_wrapper, m_buffer, m_decoder);
			f_thread.detach();

			music_start = true;
		}
		void fillBuffer() {
			m_buffer->fill_buffer(m_decoder);
		}
		void WriteWaveBuffer() {
			AudioData* data = m_buffer->display_block(w_sound->GetBlockSamples());

			if (data == nullptr) {
				music_start = false;
				return;
			}

			w_sound->WriteWaveBuffer(&data->data[0][0]);
			delete data;
			return;
		}

		int b_queueSize() {
			return m_buffer->get_buffer_queue_size();
		}

		bool next_buffer_filled() {
			return m_buffer->next_buffer_filled();
		}
		bool is_muisc_start() {
			return music_start;
		}

		void displaySpectrum(Gdiplus::Graphics* p_graphics) {
			m_buffer->displaySpectrumImg(p_graphics);
		}

		~Player() {
			w_sound->StopDevice();
			w_sound->CloseDeivce();


			if (m_decoder != nullptr) {
				delete m_decoder;
			}
			if (m_buffer != nullptr) {
				delete m_buffer;
			}

			delete w_sound;
		}
	};
}