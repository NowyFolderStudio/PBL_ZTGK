#pragma once

#include <NFSEngine.h>

#include "Components/Aura/AuraManager.hpp"
#include "Components/ScaleAnimator.hpp"

class AuraPlatform : public NFSEngine::Component {
public:
    AuraPlatform(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "AuraPlatform"; }

    AuraType RequiredAura = AuraType::First;

protected:
    NFSEngine::ColliderComponent* m_Collider = nullptr;
    ScaleAnimator* m_Animator = nullptr;
    glm::vec3 m_OriginalScale { 1.0f };

    void OnAwake() override { m_OriginalScale = GetOwner()->GetTransform()->GetScale(); }

    void OnStart() override {
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
        m_Animator = GetOwner()->GetComponent<ScaleAnimator>();

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
        bool isActive = (currentAura == RequiredAura);

        if (m_Collider) {
            m_Collider->IsTrigger = !isActive;
        }

        if (m_Animator) {
            if (isActive) {
                m_Animator->SetTargetScale(m_OriginalScale);
            } else {
                m_Animator->SetTargetScale(m_OriginalScale * 0.1f);
            }
        }
    };
};
