#pragma once

#include <NFSEngine.h>

#include "Components/Aura/AuraManager.hpp"

class AuraPlatform : public NFSEngine::Component {
public:
    AuraPlatform(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "AuraPlatform"; }

    AuraType RequiredAura = AuraType::First;

protected:
    NFSEngine::ColliderComponent* m_Collider = nullptr;

    void OnAwake() override { }

    void OnStart() override {
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();

        if (AuraManager::Instance) {
            UpdatePlatformState(AuraManager::Instance->CurrentAura);
            AuraManager::Instance->OnAuraChanged.AddListener([this](AuraType newAura) { UpdatePlatformState(newAura); });
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnEnable() override { }

    void OnDisable() override { }

private:
    void UpdatePlatformState(AuraType currentAura) {
        if (!m_Collider) return;

        if (currentAura == RequiredAura) {
            m_Collider->IsTrigger = false;
        } else {
            m_Collider->IsTrigger = true;
        }
    };
};
