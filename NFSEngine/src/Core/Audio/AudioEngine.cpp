#include <iostream>

#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "Core/Audio/AudioEngine.hpp"

#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#include "Core/Audio/AudioClip.hpp"
#include <random>

namespace NFSEngine {
    ma_engine AudioEngine::s_Engine;
    std::vector<ma_sound*> AudioEngine::s_OneShots;

    void AudioEngine::Init() {
        ma_result result = ma_engine_init(NULL, &s_Engine);

        if (result != MA_SUCCESS) {
            NFS_CORE_ERROR("MiniAudio error: {}", ma_result_description(result));
        }
    }

    void AudioEngine::Shutdown() {
        for (ma_sound* sound : s_OneShots) {
            ma_sound_uninit(sound);
            delete sound;
        }
        s_OneShots.clear();

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

    void AudioEngine::PlayClip(AudioClip* clip, float volume, float pitch) {
        if (!clip) return;

        if (clip->IsStream()) {
            NFS_CORE_WARN("Streamed audio clip ({}) should be loaded via AudioSource Component.", clip->GetFilepath());
        }

        for (auto it = s_OneShots.begin(); it != s_OneShots.end();) {
            ma_sound* sound = *it;
            if (ma_sound_at_end(sound) == MA_TRUE) {
                ma_sound_uninit(sound);
                delete sound; // Zwolnienie pamięci RAM
                it = s_OneShots.erase(it);
            } else {
                ++it;
            }
        }

        ma_sound* newSound = new ma_sound;

        ma_result result
            = ma_sound_init_from_file(&s_Engine, clip->GetFilepath().c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, newSound);

        if (result == MA_SUCCESS) {
            ma_sound_set_volume(newSound, volume);
            ma_sound_set_pitch(newSound, pitch);
            ma_sound_start(newSound);

            s_OneShots.push_back(newSound);
        } else {
            delete newSound;
            NFS_CORE_ERROR("Failed to play one shot clip: {}", clip->GetFilepath());
        }
    }

    void AudioEngine::PlayClipRandomPitch(AudioClip* clip, float minPitch, float maxPitch, float volume) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(minPitch, maxPitch);

        float randomPitch = dis(gen);

        PlayClip(clip, volume, randomPitch);
    }

    void AudioEngine::SetMasterVolume(float volume) {
        volume = std::max(std::min(volume, 2.0f), 0.0f);
        ma_engine_set_volume(&s_Engine, volume);
    }

    float AudioEngine::GetMasterVolume() { return ma_engine_get_volume(&s_Engine); }
} // namespace NFSEngine