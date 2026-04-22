#include "Components/AudioComponent.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <cmath>

namespace NFSEngine {
	AudioComponent::~AudioComponent(){
		if (m_IsLoaded) {
			ma_sound_uninit(&m_Sound);
		}
	}

	void AudioComponent::LoadSound(const std::string& filepath) {
		ma_result result = ma_sound_init_from_file(
			AudioEngine::GetEngine(),
			filepath.c_str(),
			0, NULL, NULL, &m_Sound
		);

		if (result == MA_SUCCESS) {
			m_IsLoaded = true;
			std::cout << "Dzwiek zaladowany " << std::endl;
		}
		else {
			std::cout << "Blad ladowania dxwieku" << result <<std::endl;
		}
	}

	void AudioComponent::PlayScaleTest() {
		std::thread([this]() {
			std::vector<int> cMajorScale = { 0, 2, 4, 5, 7, 9, 11, 12 };

			for (int semitones : cMajorScale) {

				float pitchMultiplier = std::pow(2.0f, semitones / 12.0f);

				ma_sound_seek_to_pcm_frame(&m_Sound, 0);
				ma_sound_set_pitch(&m_Sound, pitchMultiplier);
				ma_sound_start(&m_Sound);

				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				std::cout << "plum" << std::endl;
			}
			}).detach();
	}
}