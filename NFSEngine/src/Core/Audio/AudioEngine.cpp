#include <iostream>

#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "Core/Audio/AudioEngine.hpp"

#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#include "Core/Audio/AudioClip.hpp"

namespace NFSEngine {
    ma_engine AudioEngine::s_Engine;

    void AudioEngine::Init() {
        ma_result result = ma_engine_init(NULL, &s_Engine);

        if (result != MA_SUCCESS) {
            NFS_CORE_ERROR("MiniAudio error: {}", ma_result_description(result));
        }
    }

    void AudioEngine::Shutdown() {
        ma_engine_uninit(&s_Engine);
        NFS_CORE_INFO("MiniAudio shutdown");
    }

    ma_engine* AudioEngine::GetEngine() { return &s_Engine; }

    ma_uint64 AudioEngine::GetTimeInPCM() { return ma_engine_get_time_in_pcm_frames(&s_Engine); }

    double AudioEngine::GetGlobalTimeInSeconds() {
        ma_uint64 pcmFrames = GetTimeInPCM();
        ma_uint32 sampleRate = ma_engine_get_sample_rate(&s_Engine);

        return static_cast<double>(pcmFrames) / static_cast<double>(sampleRate);
    }

    void AudioEngine::PlayClip(AudioClip* clip) {
        if (clip && clip->IsLoaded()) {
            ma_sound_start(clip->GetSoundHandle());
        }
    }

    void AudioEngine::StopClip(AudioClip* clip) {
        if (clip && clip->IsLoaded()) {
            ma_sound_stop(clip->GetSoundHandle());
        }
    }

    void AudioEngine::RestartClip(AudioClip* clip) {
        if (clip && clip->IsLoaded()) {
            ma_sound_seek_to_pcm_frame(clip->GetSoundHandle(), 0);
            ma_sound_start(clip->GetSoundHandle());
        }
    }

    void AudioEngine::SetMasterVolume(float volume) {
        if (volume < 0.0f) volume = 0.0f;
        if (volume > 2.0f) volume = 2.0f;

        ma_engine_set_volume(&s_Engine, volume);
    }

    float AudioEngine::GetMasterVolume() { return ma_engine_get_volume(&s_Engine); }
} // namespace NFSEngine