#include "Core/AudioManager.hpp"

namespace NFSEngine
{

AudioManager::AudioManager()
    : fmodSystem(nullptr)
{
}

AudioManager::~AudioManager() { Cleanup(); }

bool AudioManager::Init()
{
    FMOD_RESULT result = FMOD::System_Create(&fmodSystem);

    if (result != FMOD_OK) return false;

    result = fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);

    if (result != FMOD_OK) return false;

    return true;
}

void AudioManager::Update()
{
    if (fmodSystem)
    {
        fmodSystem->update();
    }
}

void AudioManager::LoopSound(const std::string& soundFile, float volume)
{
    if (!fmodSystem) return;

    FMOD::Sound* sound = nullptr;

    std::string fullPath = "assets/audio/" + soundFile;

    FMOD_RESULT result = fmodSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &sound);

    if (result != FMOD_OK)
    {
        std::cerr << "[FMOD BLAD] Nie udalo sie zaladowac pliku: " << soundFile << " (Kod bledu: " << result << ")"
                  << std::endl;
        return;
    }

    FMOD::Channel* channel = nullptr;

    fmodSystem->playSound(sound, nullptr, true, &channel);

    if (channel)
    {
        channel->setVolume(volume);
        channel->setPaused(false);
    }
}

void AudioManager::Cleanup()
{
    if (fmodSystem)
    {
        fmodSystem->close();
        fmodSystem->release();
        fmodSystem = nullptr;
    }
}

}