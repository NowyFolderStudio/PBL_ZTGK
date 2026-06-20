#include "Components/AudioSource.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include <iostream>

namespace NFSEngine {

    AudioSource::~AudioSource() { CleanupSound(); }

    void AudioSource::CleanupSound() {
        if (m_IsSoundInitialized) {
            ma_sound_uninit(&m_Sound);
            m_IsSoundInitialized = false;
        }
    }

    void AudioSource::SetClip(std::shared_ptr<AudioClip> clip) {
        if (m_CurrentClip == clip) return;

        CleanupSound();
        m_CurrentClip = clip;

        if (!m_CurrentClip) return;

        ma_uint32 flags = m_CurrentClip->IsStream() ? MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;

        ma_result result = ma_sound_init_from_file(AudioEngine::GetEngine(), m_CurrentClip->GetFilepath().c_str(), flags, NULL,
                                                   NULL, &m_Sound);

        if (result == MA_SUCCESS) {
            m_IsSoundInitialized = true;
            SetVolume(m_Volume);
            SetPitch(m_Pitch);
            SetLooping(m_Loop);
        } else {
            std::cerr << "AudioSource failed to load clip: " << m_CurrentClip->GetFilepath() << std::endl;
        }
    }

    void AudioSource::Play() {
        if (m_IsSoundInitialized) {
            if (ma_sound_at_end(&m_Sound) == MA_TRUE) {
                ma_sound_seek_to_pcm_frame(&m_Sound, 0);
            }
            ma_sound_start(&m_Sound);
        }
    }

    void AudioSource::Stop() {
        if (m_IsSoundInitialized) {
            ma_sound_stop(&m_Sound);
            ma_sound_seek_to_pcm_frame(&m_Sound, 0);
        }
    }

    void AudioSource::Pause() {
        if (m_IsSoundInitialized) {
            ma_sound_stop(&m_Sound);
        }
    }

    void AudioSource::SetVolume(float volume) {
        m_Volume = volume;
        if (m_IsSoundInitialized) ma_sound_set_volume(&m_Sound, volume);
    }

    void AudioSource::SetPitch(float pitch) {
        m_Pitch = pitch;
        if (m_IsSoundInitialized) ma_sound_set_pitch(&m_Sound, pitch);
    }

    void AudioSource::SetLooping(bool loop) {
        m_Loop = loop;
        if (m_IsSoundInitialized) {
            ma_sound_set_looping(&m_Sound, loop ? MA_TRUE : MA_FALSE);
        }
    }

    bool AudioSource::IsPlaying() const {
        if (!m_IsSoundInitialized) return false;
        return ma_sound_is_playing(&m_Sound) == MA_TRUE;
    }
} // namespace NFSEngine