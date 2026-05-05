#pragma once

#include <NFSEngine.h>
#include <miniaudio/miniaudio.h>
#include <string>
#include <vector>

class InteractivePiano : public NFSEngine::Component {
public:
	InteractivePiano(NFSEngine::GameObject* owner) : Component(owner) {}
	~InteractivePiano() override {
		for (auto& sound : m_KeySounds) {
			ma_sound_uninit(&sound);
		}
	}

	std::string GetName() const override { return "InteractivePiano"; }

	void LoadPiano(const std::string& baseAudioPath) {
		std::vector<int> offsets = { 0, 2, 4, 5, 7, 9, 11 };

		m_KeySounds.resize(7);

		for (int i = 0; i < 7; i++) {
			ma_result result = ma_sound_init_from_file(NFSEngine::AudioEngine::GetEngine(), baseAudioPath.c_str(), 0, NULL, NULL, &m_KeySounds[i]);

			if (result == MA_SUCCESS) {
				float pitchMultiplier = std::pow(2.0f, offsets[i] / 12.0f);
				ma_sound_set_pitch(&m_KeySounds[i], pitchMultiplier);
			}
			else {
				std::cout << "B³¹d ³adowania dŸwiêku" << std::endl;
			}
		}

		m_IsLoaded = true;
	}

	void PlayKey(int keyIndex) {
		if (!m_IsLoaded || keyIndex < 0 || keyIndex >= 7) return;

		ma_sound_seek_to_pcm_frame(&m_KeySounds[keyIndex], 0);
		ma_sound_start(&m_KeySounds[keyIndex]);
	}

private:
	std::vector<ma_sound> m_KeySounds;
	bool m_IsLoaded = false;
};