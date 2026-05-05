#include "Core/Audio/RhythmSequencer.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include <iostream>

namespace NFSEngine {

	float RhythmSequencer::GetContinuousBeatTime() const {
		if (m_SecondsPerBeat <= 0.0) return 0.0f;
		return static_cast<float>(m_SongTime / m_SecondsPerBeat);
	}

	void RhythmSequencer::Start(float bpm, int beatsPerBar) {
		m_BPM = bpm;
		m_BeatsPerBar = beatsPerBar;

		m_SecondsPerBeat = 60.0 / static_cast<double>(m_BPM);

		m_SecondsPer16th = m_SecondsPerBeat / 4.0;

		m_SongTime = 0.0;

		m_Current16thTotal = 0;
		m_Last16thTotal = -1;
		m_CurrentBar = 1;
		m_BeatInBar = 1;
		m_16thInBeat = 1;
	}

	void RhythmSequencer::Update(float deltaTimeInSeconds) {
		m_SongTime += deltaTimeInSeconds;

		m_Current16thTotal = static_cast<int>(m_SongTime / m_SecondsPer16th);

		if (m_Current16thTotal > m_Last16thTotal) {

			m_16thInBeat = (m_Current16thTotal % 4) + 1;
			int currentBeatTotal = m_Current16thTotal / 4;
			m_BeatInBar = (currentBeatTotal % m_BeatsPerBar) + 1;
			m_CurrentBar = (currentBeatTotal / m_BeatsPerBar) + 1;

			m_Last16thTotal = m_Current16thTotal;
		}
	}
}