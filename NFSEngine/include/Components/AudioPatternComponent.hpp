#pragma once

#include "Components/Component.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Core/Audio/RhythmSequencer.hpp"
#include "Core/Audio/PatternParser.hpp"
#include <miniaudio/miniaudio.h>
#include <string>

namespace NFSEngine {
	class AudioPatternComponent : public Component {
	public:
		AudioPatternComponent(GameObject* owner) : Component(owner) {};
		~AudioPatternComponent() override;

		void LoadPattern(const std::string& patternFile, RhythmSequencer* sequencer);

		void OnUpdate(DeltaTime deltaTime) override;
		void OnImGuiRender() override;

		std::string GetName() const override { return "AudioComponent"; }

		void SetVolume(float volume);
		float GetVolume() const { return m_Volume; }

	private:
		void LoadSound(const std::string& filepath);
		void PlayNote(float pitchOffset);

		ma_sound m_Sound;
		bool m_IsLoaded = false;

		Pattern m_CurrentPattern;
		RhythmSequencer* m_Sequencer = nullptr;

		int m_LastPlayed16thTotal = -1;

		float m_Volume = 1.0f;
	};
}