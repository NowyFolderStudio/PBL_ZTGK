#pragma once

#include <NFSEngine.h>
#include <string>
#include "Core/Audio/RhythmSequencer.hpp"
#include "Core/AudioManager.hpp"
#include "Components/AudioPatternComponent.hpp"
#include "Components/Aura/AuraManager.hpp"
#include "Core/Scene.hpp"

class MusicDirector : public NFSEngine::Component {
public:
    NFSEngine::RhythmSequencer Sequencer;

    std::vector<std::string> PitchReactiveTracks = { "Bass" };

    MusicDirector(NFSEngine::GameObject* owner) : Component(owner) {}
    std::string GetName() const override { return "MusicDirector"; }

    void InitMusic(NFSEngine::Scene* scene) {
        Sequencer.Start(130.0f);

        auto* bassObj1 = scene->CreateGameObject("BassMusicPlayer1");
        auto& bassComp1 = bassObj1->AddComponent<NFSEngine::AudioPatternComponent>();
        bassComp1.TrackName = "Bass";
        bassComp1.LoadPattern("assets/audio/patterns/Disco/BassPattern01.json", &Sequencer);
        bassComp1.SetVolume(1.2f);
        NFSEngine::AudioManager::RegisterPattern(&bassComp1);

        auto* bassObj2 = scene->CreateGameObject("BassMusicPlayer2");
        auto& bassComp2 = bassObj2->AddComponent<NFSEngine::AudioPatternComponent>();
        bassComp2.TrackName = "Bass";
        bassComp2.LoadPattern("assets/audio/patterns/BassPatternPrototype2.json", &Sequencer);
        bassComp2.SetVolume(1.0f);
        NFSEngine::AudioManager::RegisterPattern(&bassComp2);

        auto* pianoObj = scene->CreateGameObject("PianoMusicPlayer1");
        auto& audioComp = pianoObj->AddComponent<NFSEngine::AudioPatternComponent>();
        audioComp.TrackName = "Piano";
        audioComp.LoadPattern("assets/audio/patterns/Disco/PianoPattern01.json", &Sequencer);
        audioComp.SetVolume(0.4f);
        NFSEngine::AudioManager::RegisterPattern(&audioComp);

        auto* pianoObj2 = scene->CreateGameObject("PianoMusicPlayer2");
        auto& audioComp2 = pianoObj2->AddComponent<NFSEngine::AudioPatternComponent>();
        audioComp2.TrackName = "Piano";
        audioComp2.LoadPattern("assets/audio/patterns/PianoPattern2.json", &Sequencer);
        audioComp2.SetVolume(0.5f);
        NFSEngine::AudioManager::RegisterPattern(&audioComp2);

        auto* subPianoObj = scene->CreateGameObject("SubPianoMusicPlayer1");
        auto& subPianoComp = subPianoObj->AddComponent<NFSEngine::AudioPatternComponent>();
        subPianoComp.TrackName = "SubPiano";
        subPianoComp.LoadPattern("assets/audio/patterns/Disco/SubPiano01.json", &Sequencer);
        subPianoComp.SetVolume(1.1f);
        NFSEngine::AudioManager::RegisterPattern(&subPianoComp);

        auto* kickObj = scene->CreateGameObject("KickMusicPlayer1");
        auto& kickComp = kickObj->AddComponent<NFSEngine::AudioPatternComponent>();
        kickComp.TrackName = "Kick";
        kickComp.LoadPattern("assets/audio/patterns/Disco/KickPattern01.json", &Sequencer);
        kickComp.SetVolume(1.0f);
        NFSEngine::AudioManager::RegisterPattern(&kickComp);

        auto* clapObj = scene->CreateGameObject("ClapMusicPlayer");
        auto& clapComp = clapObj->AddComponent<NFSEngine::AudioPatternComponent>();
        clapComp.TrackName = "Clap";
        clapComp.LoadPattern("assets/audio/patterns/Disco/ClapPattern01.json", &Sequencer);
        clapComp.SetVolume(1.0f);
        NFSEngine::AudioManager::RegisterPattern(&clapComp);

        auto* hitHatObj = scene->CreateGameObject("HitHatMusicPlayer");
        auto& hitHatComp = hitHatObj->AddComponent<NFSEngine::AudioPatternComponent>();
        hitHatComp.TrackName = "HitHat";
        hitHatComp.LoadPattern("assets/audio/patterns/Disco/HitHatPattern01.json", &Sequencer);
        hitHatComp.SetVolume(1.0f);
        NFSEngine::AudioManager::RegisterPattern(&hitHatComp);

        auto* snareObj = scene->CreateGameObject("snareMusicPlayer");
        auto& snareComp = snareObj->AddComponent<NFSEngine::AudioPatternComponent>();
        snareComp.TrackName = "Snare";
        snareComp.LoadPattern("assets/audio/patterns/Disco/SnarePattern01.json", &Sequencer);
        snareComp.SetVolume(2.0f);
        NFSEngine::AudioManager::RegisterPattern(&snareComp);

        NFSEngine::AudioManager::SetActivePatternInTrack("Kick", "KickPattern01");
        NFSEngine::AudioManager::SetActivePatternInTrack("Clap", "ClapPattern01");
        NFSEngine::AudioManager::SetActivePatternInTrack("HitHat", "HitHatPattern01");
        NFSEngine::AudioManager::SetActivePatternInTrack("Snare", "SnarePattern01");

        NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPattern01");
        NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern1");
        NFSEngine::AudioManager::SetActivePatternInTrack("SubPiano", "SubPiano01");

        NFSEngine::AudioManager::MuteTrack("Bass", true);
        NFSEngine::AudioManager::MuteTrack("Piano", true);
        NFSEngine::AudioManager::MuteTrack("SubPiano", true);
    }

    void OnStart() override {
        if (AuraManager::Instance) {
            m_LastKnownAura = AuraManager::Instance->CurrentAura;
            ApplyAuraStateImmediate(m_LastKnownAura);
        }

        if (LivesManager::Instance) {
            m_LastKnownLives = LivesManager::Instance->GetLives();
            ApplyHealthStateImmediate(m_LastKnownLives);
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        Sequencer.Update((float)deltaTime);
        NFSEngine::AudioManager::Update(deltaTime);

        if (AuraManager::Instance && AuraManager::Instance->CurrentAura != m_LastKnownAura) {
            m_LastKnownAura = AuraManager::Instance->CurrentAura;
            ApplyAuraStateImmediate(m_LastKnownAura);
        }

        if (LivesManager::Instance && LivesManager::Instance->GetLives() != m_LastKnownLives) {
            m_LastKnownLives = LivesManager::Instance->GetLives();
            ApplyHealthStateImmediate(m_LastKnownLives);
        }
    }

    void UnlockTrack(const std::string& trackName) {
        if (trackName.empty()) return;
        NFSEngine::AudioManager::MuteTrack(trackName, false);
    }

private:
    AuraType m_LastKnownAura = AuraType::First;
    int m_LastKnownLives = 3;

    void ApplyAuraStateImmediate(AuraType aura) {
        if (aura == AuraType::First) {
            NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPattern01");
            NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern1");
            NFSEngine::AudioManager::SetActivePatternInTrack("SubPiano", "SubPiano01");
        }
        else if (aura == AuraType::Second) {
            NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPatternPrototype2");
            NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern2");
        }
    }

    void ApplyHealthStateImmediate(int currentLives) {
        float pitchModifier = 0.0f;

        if (currentLives == 2) pitchModifier = 12.0f;
        else if (currentLives <= 1) pitchModifier = 24.0f;

        for (const auto& trackName : PitchReactiveTracks) {
            NFSEngine::AudioManager::SetTrackPitchOffset(trackName, pitchModifier);
        }
    }

    
};