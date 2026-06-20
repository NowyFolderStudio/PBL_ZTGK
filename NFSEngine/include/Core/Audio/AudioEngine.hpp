#pragma once
#include <miniaudio/miniaudio.h>

namespace NFSEngine {
    class AudioClip;

    class AudioEngine {
    public:
        static void Init();
        static void Shutdown();

        static ma_engine* GetEngine();

        static double GetGlobalTimeInSeconds();
        static ma_uint64 GetTimeInPCM();

        static void PlayClip(AudioClip* clip);
        static void StopClip(AudioClip* clip);
        static void RestartClip(AudioClip* clip);

        static void SetMasterVolume(float volume);
        static float GetMasterVolume();

    private:
        static ma_engine s_Engine;
    };
} // namespace NFSEngine