#pragma once
#include <miniaudio/miniaudio.h>
#include "Components/Component.hpp"
#include "Core/Audio/AudioClip.hpp"
#include <memory>

namespace NFSEngine {
    class AudioSource : public Component {
    public:
        AudioSource(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "AudioSource"; }

        ~AudioSource();

        AudioSource(const AudioSource&) = delete;
        AudioSource& operator=(const AudioSource&) = delete;

        void SetClip(std::shared_ptr<AudioClip> clip);

        void Play();
        void Stop();
        void Pause();

        void SetVolume(float volume);
        void SetPitch(float pitch);
        void SetLooping(bool loop);

        bool IsPlaying() const;

    private:
        void CleanupSound();

        ma_sound m_Sound;
        std::shared_ptr<AudioClip> m_CurrentClip;
        bool m_IsSoundInitialized = false;

        float m_Volume = 1.0f;
        float m_Pitch = 1.0f;
        bool m_Loop = false;
    };
} // namespace NFSEngine