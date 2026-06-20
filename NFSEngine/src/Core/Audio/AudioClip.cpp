#include "Core/Audio/AudioClip.hpp"
#include "Core/Audio/AudioEngine.hpp" // Aby pobrać instancję ma_engine
// #include "Core/Log.hpp" // Twoje logowanie (NFS_CORE_ERROR itp.)

namespace NFSEngine {

    AudioClip::AudioClip(const std::string& filepath, bool stream)
        : m_Filepath(filepath) {
        ma_uint32 flags = stream ? MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;

        ma_result result = ma_sound_init_from_file(AudioEngine::GetEngine(), filepath.c_str(), flags, NULL, NULL, &m_Sound);

        if (result == MA_SUCCESS) {
            m_IsLoaded = true;
        } else {
            NFS_CORE_ERROR("MiniAudio failed to load clip {}: {}", filepath, ma_result_description(result));
        }
    }

    AudioClip::~AudioClip() {
        if (m_IsLoaded) {
            ma_sound_uninit(&m_Sound);
        }
    }

    void AudioClip::SetVolume(float volume) {
        if (m_IsLoaded) ma_sound_set_volume(&m_Sound, volume);
    }

    void AudioClip::SetPitch(float pitch) {
        if (m_IsLoaded) ma_sound_set_pitch(&m_Sound, pitch);
    }

    void AudioClip::SetLooping(bool loop) {
        if (m_IsLoaded) ma_sound_set_looping(&m_Sound, loop ? MA_TRUE : MA_FALSE);
    }

    float AudioClip::GetVolume() const { return m_IsLoaded ? ma_sound_get_volume(&m_Sound) : 0.0f; }

    float AudioClip::GetPitch() const { return m_IsLoaded ? ma_sound_get_pitch(&m_Sound) : 1.0f; }

    bool AudioClip::IsLooping() const { return m_IsLoaded ? (ma_sound_is_looping(&m_Sound) == MA_TRUE) : false; }

    float AudioClip::GetDurationInSeconds() const {
        if (!m_IsLoaded) return 0.0f;
        float length;
        ma_sound_get_length_in_seconds(&m_Sound, &length);
        return length;
    }

    bool AudioClip::IsPlaying() const { return m_IsLoaded ? (ma_sound_is_playing(&m_Sound) == MA_TRUE) : false; }

    ma_sound* AudioClip::GetSoundHandle() { return &m_Sound; }

} // namespace NFSEngine