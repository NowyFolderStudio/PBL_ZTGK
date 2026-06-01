#pragma once

#include <NFSEngine.h>

#include "Components/Aura/AuraManager.hpp"

class CasetteComponent : public NFSEngine::Component {
public:
    explicit CasetteComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "CasetteComponent"; }

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Collected) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            m_Collected = true;

            if (AuraManager::Instance) {
                AuraManager::Instance->UnlockNextAura();
            }

            m_Owner->SetActive(false);
        };
    }

private:
    bool m_Collected = false;
};
