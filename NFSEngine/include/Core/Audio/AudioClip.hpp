#pragma once
#include <string>
#include <miniaudio/miniaudio.h>

namespace NFSEngine {
    class AudioClip {
    public:
        // Use stream = true for longer audio clips to save up RAM
        // Use stream = false for short clips
        AudioClip(const std::string& filepath, bool stream = false);
        ~AudioClip();

        // This class is not allowed to be copied
        AudioClip(const AudioClip&) = delete;
        AudioClip& operator=(const AudioClip&) = delete;

        void SetVolume(float volume);
        void SetPitch(float pitch);
        void SetLooping(bool loop);

        float GetVolume() const;
        float GetPitch() const;
        bool IsLooping() const;
        float GetDurationInSeconds() const;
        bool IsPlaying() const;

        ma_sound* GetSoundHandle();
        bool IsLoaded() const { return m_IsLoaded; }

    private:
        ma_sound m_Sound;
        std::string m_Filepath;
        bool m_IsLoaded = false;
    };
} // namespace NFSEngine