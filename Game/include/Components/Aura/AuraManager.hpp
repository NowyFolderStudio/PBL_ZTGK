#pragma once

#include <NFSEngine.h>
#include "Core/AudioManager.hpp"

enum class AuraType { // TODO: Rename it
    None,
    First,
    Second,
    Third
};

class AuraManager : public NFSEngine::Component {
public:
    inline static AuraManager* Instance = nullptr;

    AuraManager(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~AuraManager() override {
        if (Instance == this) {
            Instance = nullptr;
        }
    }

    std::string GetName() const override { return "AuraManager"; }

    NFSEngine::Action<AuraType> OnAuraChanged;
    AuraType CurrentAura = AuraType::None;

private:
    std::vector<AuraType> m_UnlockedAuras;

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) {
            return;
        }
        Instance = this;
    }

    void OnStart() override {

        // For testing purposes
        UnlockAura(AuraType::First);
        UnlockAura(AuraType::Second);
        UnlockAura(AuraType::Third);
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnEnable() override { }

    void OnDisable() override { }

public:
    void UnlockAura(AuraType aura) {
        if (std::find(m_UnlockedAuras.begin(), m_UnlockedAuras.end(), aura) == m_UnlockedAuras.end()) {
            m_UnlockedAuras.push_back(aura);
        }
    }

    void UnlockNextAura() {
        static const std::vector<AuraType> progressionOrder = { AuraType::First, AuraType::Second, AuraType::Third };

        size_t currentProgress = m_UnlockedAuras.size();

        if (currentProgress < progressionOrder.size()) {
            AuraType nextAura = progressionOrder[currentProgress];

            UnlockAura(nextAura);
        }
    }

    bool IsAuraUnlocked(AuraType aura) const {
        return std::find(m_UnlockedAuras.begin(), m_UnlockedAuras.end(), aura) != m_UnlockedAuras.end();
    }

    void ChangeAura(AuraType newAura) {
        if (CurrentAura == newAura) return;

        if (!IsAuraUnlocked(newAura)) {
            return;
        }

        if (CurrentAura == AuraType::First) {
            NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPatternPrototype");
            NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern1");
        }

        if (CurrentAura == AuraType::Second) {
            NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPatternPrototype2");
            NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern2");
        }

        CurrentAura = newAura;

        OnAuraChanged.Invoke(newAura);
    }
};