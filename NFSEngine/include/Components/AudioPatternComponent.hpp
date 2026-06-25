#pragma once

#include "Components/Component.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Core/Audio/RhythmSequencer.hpp"
#include "Core/Audio/PatternParser.hpp"

#include <miniaudio/miniaudio.h>
#include <string>
#include <vector>

namespace NFSEngine {
    class AudioPatternComponent : public Component {
    public:
        float GlobalPitchModifier = 0.0f;

        AudioPatternComponent(GameObject* owner)
            : Component(owner) { };
        ~AudioPatternComponent() override;

        void LoadPattern(const std::string& patternFile, RhythmSequencer* sequencer);

        void OnUpdate(DeltaTime deltaTime) override;
        void OnImGuiRender() override;

        void SetVolume(float volume);
        void SetMute(bool mute);
        void SetActive(bool active);

        float GetVolume() const { return m_Volume; }
        std::string GetName() const override { return "AudioComponent"; }
        std::string GetPatternName() const { return m_CurrentPattern.name; }

        Pattern GetPattern() const { return m_CurrentPattern; }
        RhythmSequencer* GetSequencer() const { return m_Sequencer; }

        std::string TrackName = "Default";
        bool IsActive = true;

    private:
        struct VoiceData {
            bool isPlaying = false;
            bool isFading = false;
            float fadeTimer = 0.0f;
            int stop16thTotal = 0;
        };

        void LoadSound(const std::string& filepath);
        void PlayNote(float pitchOffset, int lengthIn16ths);

        std::vector<ma_sound> m_Voices;
        std::vector<VoiceData> m_VoiceData;
        int m_CurrentVoiceIndex = 0;

        bool m_IsLoaded = false;

        Pattern m_CurrentPattern;
        RhythmSequencer* m_Sequencer = nullptr;

        int m_LastPlayed16thTotal = -1;

        float m_Volume = 1.0f;

        bool m_IsMuted = false;
        float m_UnmutedVolume = 1.0f;
    };
} // namespace NFSEngine