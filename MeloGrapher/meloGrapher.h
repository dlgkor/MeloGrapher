#pragma once
#include"common.h"
#include"ffmpegWrapper.h"
#include"audioUtils.h"
#include"spectrumUtils.h"
#include"FFT.h"

#define MAX_MELO_BUFFER 8

namespace melo {
	class BufferWrapper {
	private:
		AudioData audio_buffer[MAX_MELO_BUFFER];
		bool audio_filled[MAX_MELO_BUFFER];
		std::queue<int> empty_audio_buffer;

		audioCapacitor capacitor;

		int channels;
		int p_Sample; //buffer size

		int target_buffer;
		long long int cursor; //cursor which means the time of music currently reading

		int end_buffer; //end buffer which is activated when buffer reads EOF


		SpectrumOption spectrum_option;
		SpectrumVideo spectrum_buffer[MAX_MELO_BUFFER];
		bool spectrum_filled[MAX_MELO_BUFFER];

	public:
		BufferWrapper(int _channels, int _p_Sample)
			: channels(_channels), p_Sample(_p_Sample) {

			target_buffer = 0;
			cursor = 0;
			end_buffer = -1;

			for (int i = 0; i < MAX_MELO_BUFFER; i++) {
				audio_buffer[i].Init(channels, p_Sample);
				audio_filled[i] = false;
				spectrum_filled[i] = false;
			}

			capacitor.InitAudio(channels, 10000);

			for (int i = 0; i < MAX_MELO_BUFFER; i++) {
				empty_audio_buffer.push(i);
			}
		}

		void set_spectrum_option(SpectrumOption _spectrum_option) {
			spectrum_option = _spectrum_option;
		}

		AudioData* display_block(int blockSize) {
			/*
			*print wave and spectrum, if exists
			*check if buffer is changed
			*if there is no block to display wait for 8 buffers to be filled or end_buffer activates
			*return audioData which contains block
			*/

			AudioData* audio_block = new AudioData(channels, blockSize);

			if (target_buffer == end_buffer) {
				//end of the block
				return nullptr;
			}

			if (audio_buffer[target_buffer].cur + blockSize > audio_buffer[target_buffer].n_samples) {
				int block_size1 = audio_buffer[target_buffer].n_samples - audio_buffer[target_buffer].cur;
				for (int channel = 0; channel < channels; channel++) {
					for (int sample = 0; sample < block_size1; sample++) {
						audio_block->data[channel][sample] = audio_buffer[target_buffer].data[channel][audio_buffer[target_buffer].cur + sample];
					}
				}

				audio_block->cur += block_size1;
				audio_buffer[target_buffer].cur = audio_buffer[target_buffer].n_samples;

				int next_target_buffer = (target_buffer + 1) % MAX_MELO_BUFFER;

				for (int channel = 0; channel < channels; channel++) {
					for (int sample = 0; sample < blockSize - block_size1; sample++) {
						audio_block->data[channel][audio_block->cur + sample] = audio_buffer[next_target_buffer].data[channel][sample];
					}
				}

				audio_block->cur += blockSize - block_size1;
				audio_buffer[next_target_buffer].cur = blockSize - block_size1;

				audio_filled[target_buffer] = false;
				audio_buffer[target_buffer].cur = 0;

				empty_audio_buffer.push(target_buffer);

				target_buffer = next_target_buffer;
			}
			else {
				for (int channel = 0; channel < channels; channel++) {
					for (int sample = 0; sample < blockSize; sample++) {
						audio_block->data[channel][sample] = audio_buffer[target_buffer].data[channel][audio_buffer[target_buffer].cur + sample];
					}
				}

				audio_buffer[target_buffer].cur += blockSize;
			}

			cursor += blockSize;

			return audio_block;
		}

		void fill_buffer(mp3Decoder* target_mp3) {
			/*
			*fill buffer if buffer is empty
			*use audioCapacitor in audiobuffer.h
			*get audioData from mp3decode in ffmpegWrapper.h
			*I think filled buffer will be 1 second(44100 sample)
			*/
			for (int i = 0; i < MAX_MELO_BUFFER; i++) {
				int current_target = (target_buffer + i) % MAX_MELO_BUFFER;

				if (audio_filled[current_target])
					continue;

				int ref = capacitor.Pull(&audio_buffer[current_target], p_Sample);
				if (ref == p_Sample) {
					audio_filled[current_target] = true;
					fill_spectrum_buffer(current_target);
					continue;
				}
				//Pull from capacitor

				int missing_size = p_Sample - ref;
				AudioData* audio = target_mp3->decode_mp3(missing_size);

				if (audio == nullptr) {
					end_buffer = current_target;
					return;
				}

				for (int channel = 0; channel < channels; channel++) {
					memcpy_s(&audio_buffer[current_target].data[channel][ref], sizeof(short) * missing_size,
						&audio->data[channel][0], sizeof(short) * missing_size);
				}

				//get deficient audio data from mp3 decoder
				audio_buffer[current_target].resetCursor();


				AudioData remain_audio(channels, 10000);
				int remain_size = audio->n_samples - missing_size;
				for (int channel = 0; channel < channels; channel++) {
					for (int j = 0; j < remain_size; j++) {
						remain_audio.data[channel][j] = audio->data[channel][missing_size + j];
					}
				}

				capacitor.Push(remain_audio, remain_size);
				//save remaning audio data from mp3 decoder

				delete audio;

				audio_filled[current_target] = true;

				//activate function check_spectrum continuously
				// 
				fill_spectrum_buffer(current_target);
			}
		}

		void fill_empty_buffer(mp3Decoder* target_mp3) {
			/*
			*fill buffer if buffer is empty
			*use audioCapacitor in audiobuffer.h
			*get audioData from mp3decode in ffmpegWrapper.h
			*I think filled buffer will be 1 second(44100 sample)
			*/
			if (empty_audio_buffer.size() == 0) {
				return;
			}

			int current_target = empty_audio_buffer.front();
			empty_audio_buffer.pop();

			if (audio_filled[current_target])
				return;

			int ref = capacitor.Pull(&audio_buffer[current_target], p_Sample);
			if (ref == p_Sample) {
				audio_filled[current_target] = true;
				fill_spectrum_buffer(current_target);
				return;
			}
			//Pull from capacitor

			int missing_size = p_Sample - ref;
			AudioData* audio = target_mp3->decode_mp3(missing_size);

			if (audio == nullptr) {
				end_buffer = current_target;
				return;
			}

			for (int channel = 0; channel < channels; channel++) {
				memcpy_s(&audio_buffer[current_target].data[channel][ref], sizeof(short) * missing_size,
					&audio->data[channel][0], sizeof(short) * missing_size);
			}

			//get deficient audio data from mp3 decoder
			audio_buffer[current_target].resetCursor();

			AudioData remain_audio(channels, 10000);
			int remain_size = audio->n_samples - missing_size;
			for (int channel = 0; channel < channels; channel++) {
				for (int j = 0; j < remain_size; j++) {
					remain_audio.data[channel][j] = audio->data[channel][missing_size + j];
				}
			}

			capacitor.Push(remain_audio, remain_size);
			//save remaning audio data from mp3 decoder

			delete audio;

			audio_filled[current_target] = true;

			//activate function check_spectrum continuously
			
			fill_spectrum_buffer(current_target);
		}

		static void fill_buffer_wrapper(BufferWrapper* obj, mp3Decoder* target_mp3) {
			while (1) {
				if (obj->music_ended())
					return;

				obj->fill_empty_buffer(target_mp3);
			}
		}

		void fill_spectrum_buffer(int current_target) {
			// check spectrum form before buffer and current buffer
			// check untill window right end cross the limit

			int back_buffer = (current_target - 1 + MAX_MELO_BUFFER) % MAX_MELO_BUFFER;

			int b_cur;
			if (spectrum_buffer[back_buffer].data.size() != 0)
				b_cur = spectrum_buffer[back_buffer].data.back().s_location + spectrum_option.s_gap;
			else
				b_cur = p_Sample + spectrum_option.s_window_half;

			//complete unfinished back_buffer filling with target_buffer
			int sample = 0;
			for (sample = b_cur;; sample += spectrum_option.s_gap) {
				if (sample - spectrum_option.s_window_half >= p_Sample)
					break;
				// FFT
				// make vector buffer for fft
				// slice it from audiodata
				std::vector<cpx> audio_slice;
				SpectrumImage spectrum_slice;

				int back_start_point = sample - spectrum_option.s_window_half;
				int back_slice_size = p_Sample - back_start_point;
				for (int i = 0; i < back_slice_size; i++) {
					audio_slice.push_back(cpx((double)audio_buffer[back_buffer].data[0][back_start_point + i], 0.0));
				}


				int target_slice_size = spectrum_option.s_window - back_slice_size;
				for (int i = 0; i < target_slice_size; i++) {
					audio_slice.push_back(cpx((double)audio_buffer[current_target].data[0][i], 0.0));
				}
				// need to concern target_buffer because fft window will inevitably invade it
				// s_location = sample

				FFT(audio_slice, false);

				spectrum_slice.data = audio_slice;
				spectrum_slice.s_location = sample;

				spectrum_buffer[back_buffer].data.push_back(spectrum_slice);				
			}
			//set spectrum_filled[back_buffer] to true
			spectrum_filled[back_buffer] = true;

			spectrum_buffer[current_target].data.clear(); //clear target spectrum buffer
			
			//target_buffer fft
			sample -= p_Sample;
			for (;; sample += spectrum_option.s_gap) {
				if (sample + spectrum_option.s_window_half >= p_Sample)
					break;

				std::vector<cpx> audio_slice;
				SpectrumImage spectrum_slice;

				//slice audiodata to audio_slice
				int target_start_point = sample - spectrum_option.s_window_half;
				for (int i = 0; i < spectrum_option.s_window; i++) {
					audio_slice.push_back(cpx((double)audio_buffer[current_target].data[0][target_start_point + i], 0.0));
				}

				//FFT
				FFT(audio_slice, false);

				spectrum_slice.data = audio_slice;
				spectrum_slice.s_location = sample; // s_location = sample

				spectrum_buffer[current_target].data.push_back(spectrum_slice);
			}

			spectrum_filled[current_target] = false;
		}

		bool next_buffer_filled() {
			//check before displaying music and spectrum
			//if it is false wait until the buffer is filled

			int next_target_buffer = (target_buffer + 1) % MAX_MELO_BUFFER;

			return !(audio_filled[next_target_buffer] == false || spectrum_filled[next_target_buffer] == false);
		}

		SpectrumImage* get_spectrum_img() {
			//check spectrumimg from cursur until it's s_location overs current audio cur(sample)
			//change cursur
			//return spectrum img


		}

		void clear_all_buffer() {
			//clear all buffer(audio and spectrum)
			//activated when cursor is moved from the mp3decoder or object is initiated
			for (int i = 0; i < MAX_MELO_BUFFER; i++) {
				audio_buffer[i].ClearData();
				spectrum_buffer[i].ClearData();

				audio_filled[i] = false;
				spectrum_filled[i] = false;
			}

			target_buffer = 0;
			cursor = 0;
			end_buffer = -1;
		}

		void moveCursor(long long int sample_location) {
			//move mp3 decoder cursur from the outsid mp3Decoder class
			clear_all_buffer();

			cursor = sample_location;
		}

		bool music_ended() {
			if (end_buffer == target_buffer)
				return true;

			return false;
		}

		~BufferWrapper() {

		}
	};

}