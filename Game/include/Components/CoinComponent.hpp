#pragma once

#include <NFSEngine.h>

#include "Components/Managers/ScoreManager.hpp"

class CoinComponent : public NFSEngine::Component {
public:
    explicit CoinComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "CoinComponent"; }

    int ScoreValue = 67;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Collected) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            m_Collected = true;

            if (ScoreManager::Instance) {
                ScoreManager::Instance->AddScore(ScoreValue);
            }

            m_Owner->SetActive(false);
        };
    }

private:
    bool m_Collected = false;
};
