#pragma once

namespace NFSEngine {
	class RhythmSequencer {
	public:
		RhythmSequencer() = default;

		void Start(float bpm, int beatsPerBar = 4);

		void Update(float deltaTimeInSeconds);

		int GetCurrentBar() const { return m_CurrentBar; }
		int GetBeatInBar() const { return m_BeatInBar; }
		int Get16thInBeat() const { return m_16thInBeat;  }
		int GetCurrent16thTotal() const { return m_Current16thTotal; }
		float GetContinuousBeatTime() const;

	private:
		float m_BPM = 120.0f;
		int m_BeatsPerBar = 4;

		double m_SecondsPerBeat = 0.5;
		double m_SecondsPer16th = 0.125;

		double m_SongTime = 0.0;

		int m_Current16thTotal = 0;
		int m_Last16thTotal = -1;

		int m_CurrentBar = 1;
		int m_BeatInBar = 1;
		int m_16thInBeat = 1;
	};
}