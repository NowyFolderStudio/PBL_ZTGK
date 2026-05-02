#include "Components/AudioPatternComponent.hpp"
#include <iostream>
#include <cmath>

namespace NFSEngine {

	AudioPatternComponent::~AudioPatternComponent() {
		if (m_IsLoaded) {
			ma_sound_uninit(&m_Sound);
		}
	}

	void AudioPatternComponent::LoadSound(const std::string& filepath) {

		std::cout << filepath << std::endl;

		ma_result result = ma_sound_init_from_file(
			AudioEngine::GetEngine(),
			filepath.c_str(),
			0, NULL, NULL, &m_Sound
		);
		m_IsLoaded = (result == MA_SUCCESS);
		if (m_IsLoaded) {
			std::cout << "git" << std::endl;
		}
		else {
			std::cout << "REsult wczytywania:   " << result << std::endl;
		}
		
	}

	void AudioPatternComponent::LoadPattern(const std::string& patternFile, RhythmSequencer* sequencer) {
		m_Sequencer = sequencer;

		m_CurrentPattern = PatternParser::LoadFromFile(patternFile);

		if (!m_CurrentPattern.audioFile.empty()) {
			LoadSound(m_CurrentPattern.audioFile);
			std::cout << "Zaladowano instrument: " << m_CurrentPattern.audioFile << std::endl;
		}
	}

	void AudioPatternComponent::PlayNote(float pitchOffset) {
		if (!m_IsLoaded) return;

		float pitchMultiplier = std::pow(2.0f, pitchOffset / 12.0f);
		ma_sound_seek_to_pcm_frame(&m_Sound, 0);
		ma_sound_set_pitch(&m_Sound, pitchMultiplier);
		ma_sound_start(&m_Sound);
	}

	void AudioPatternComponent::OnUpdate(DeltaTime deltaTime) {
		if (!m_Sequencer || !m_IsLoaded || m_CurrentPattern.totalBars <= 0) return;

		int current16thTotal = m_Sequencer->GetCurrent16thTotal();

		if (current16thTotal > m_LastPlayed16thTotal) {

			int absoluteBar = m_Sequencer->GetCurrentBar();
			int currentBeat = m_Sequencer->GetBeatInBar();
			int current16th = m_Sequencer->Get16thInBeat();

			int localBar = ((absoluteBar - 1) % m_CurrentPattern.totalBars) + 1;

			for (const auto& note : m_CurrentPattern.notes) {
				if (note.bar == localBar && note.beat == currentBeat && note.sixteenth == current16th) {

					PlayNote(note.pitchOffset);
					std::cout << "[AUDIO] Gram nute na takcie zloopowanym " << localBar << " (absolutny: " << absoluteBar << "), bicie " << currentBeat << std::endl;
				}
			}

			m_LastPlayed16thTotal = current16thTotal;
		}
	}
}