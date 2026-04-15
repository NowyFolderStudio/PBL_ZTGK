#pragma once

#include <fmod.hpp>
#include <string>
#include <iostream>

namespace NFSEngine {

    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        bool Init();
        void Update();
        void LoopSound(const std::string& soundFile, float volume);
        void Cleanup();

    private:
        FMOD::System* fmodSystem;
    };

} // namespace NFSEngine