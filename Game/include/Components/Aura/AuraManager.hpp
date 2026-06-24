#pragma once

#include <NFSEngine.h>
#include <memory>
#include <algorithm>
#include "Core/Audio/AudioClip.hpp"
#include "Core/Audio/AudioEngine.hpp"

enum class AuraType {
    First,
    Second
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
    AuraType CurrentAura = AuraType::First;

    float CooldownDuration = 1.0f;

private:
    std::vector<AuraType> m_UnlockedAuras;
    std::shared_ptr<NFSEngine::AudioClip> m_AudioClip;

    float m_CurrentCooldown = 0.0f;

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) {
            return;
        }
        Instance = this;
        m_AudioClip = std::make_shared<NFSEngine::AudioClip>("assets/audio/sounds/dj_scratch.mp3");
    }

    void OnStart() override {
        UnlockAura(AuraType::First);
        UnlockAura(AuraType::Second);
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_CurrentCooldown > 0.0f) {
            m_CurrentCooldown -= deltaTime.GetSeconds();
            if (m_CurrentCooldown < 0.0f) {
                m_CurrentCooldown = 0.0f;
            }
        }
    }

    void OnEnable() override { }
    void OnDisable() override { }

public:
    void UnlockAura(AuraType aura) {
        if (std::find(m_UnlockedAuras.begin(), m_UnlockedAuras.end(), aura) == m_UnlockedAuras.end()) {
            m_UnlockedAuras.push_back(aura);
        }
    }

    void UnlockNextAura() {
        static const std::vector<AuraType> progressionOrder = { AuraType::First, AuraType::Second };
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
        if (!IsAuraUnlocked(newAura)) return;

        if (m_CurrentCooldown > 0.0f) return;

        NFSEngine::AudioEngine::PlayClipRandomPitch(m_AudioClip.get(), 0.9, 1.1);

        CurrentAura = newAura;
        m_CurrentCooldown = CooldownDuration; // Resetujemy cooldown po udanej zmianie!

        OnAuraChanged.Invoke(newAura);
    }

    float GetCooldownProgress() const {
        if (CooldownDuration <= 0.0f) return 1.0f;

        return 1.0f - (m_CurrentCooldown / CooldownDuration);
    }
};