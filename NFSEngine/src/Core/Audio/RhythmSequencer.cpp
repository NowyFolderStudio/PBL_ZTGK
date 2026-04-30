#include "Core/Audio/RhythmSequencer.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include <iostream>

namespace NFSEngine {

	void RhythmSequencer::Start(float bpm, int beatsPerBar) {
		m_BPM = bpm;
		m_BeatsPerBar = beatsPerBar;

		m_SecondsPerBeat = 60.0 / static_cast<double>(m_BPM);

		m_SecondsPer16th = m_SecondsPerBeat / 4.0;

		m_StartTime = AudioEngine::GetGlobalTimeInSeconds();

		m_Current16thTotal = 0;
		m_Last16thTotal = -1;
		m_CurrentBar = 1;
		m_BeatInBar = 1;
		m_16thInBeat = 1;
	}

	void RhythmSequencer::Update() {
		double songTime = AudioEngine::GetGlobalTimeInSeconds() - m_StartTime;

		if (songTime < 0) return;

		m_Current16thTotal = static_cast<int>(songTime / m_SecondsPer16th);

		if (m_Current16thTotal > m_Last16thTotal) {

			m_16thInBeat = (m_Current16thTotal % 4) + 1;
			int currentBeatTotal = m_Current16thTotal / 4;
			m_BeatInBar = (currentBeatTotal % m_BeatsPerBar) + 1;
			m_CurrentBar = (currentBeatTotal / m_BeatsPerBar) + 1;

			if (m_16thInBeat == 1) {
				std::cout << "[SEKWENCER] Takt: " << m_CurrentBar << " | Bit: " << m_BeatInBar << std::endl;
			}
			else {
				std::cout << "[SEKWENCER]    ... szesnastka " << m_16thInBeat << std::endl;
			}

			m_Last16thTotal = m_Current16thTotal;
		}
	}
}