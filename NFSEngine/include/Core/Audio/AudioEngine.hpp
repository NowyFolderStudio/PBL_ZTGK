#pragma once
#include <miniaudio/miniaudio.h>
#include <vector>

namespace NFSEngine {
    class AudioClip;

    class AudioEngine {
    public:
        static void Init();
        static void Shutdown();

        static ma_engine* GetEngine();

        static double GetGlobalTimeInSeconds();
        static ma_uint64 GetTimeInPCM();

        static void PlayClip(AudioClip* clip, float volume = 1.0f, float pitch = 1.0f);

        static void PlayClipRandomPitch(AudioClip* clip, float minPitch = 0.8f, float maxPitch = 1.2f, float volume = 1.0f);

        static void SetMasterVolume(float volume);
        static float GetMasterVolume();

    private:
        static ma_engine s_Engine;

        static std::vector<ma_sound*> s_OneShots;
    };
} // namespace NFSEngine