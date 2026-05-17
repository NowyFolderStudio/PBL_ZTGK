#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Components/ScoreManagerComponent.hpp"
#include "Core/Scene.hpp"

namespace NFSEngine {

    class CoinComponent : public Component {
    public:
        explicit CoinComponent(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "CoinComponent"; }

        int ScoreValue = 1000;

    protected:
        void OnStart() override {
            auto* collider = m_Owner->GetComponent<ColliderComponent>();
            if (!collider) return;
            collider->IsTrigger = true;

            auto* scene = m_Owner->GetScene();
            auto* gm = scene ? scene->FindWithTag(Tags::ScoreManager) : nullptr;
            auto* scoreComp = gm ? gm->GetComponent<ScoreManagerComponent>() : nullptr;

            collider->OnTriggerEnter = [this, scoreComp](GameObject* other) {
                if (m_Collected) return;
                if (!other->CompareTag(Tags::Player)) return;
                m_Collected = true;
                if (scoreComp) scoreComp->AddScore(ScoreValue);
                m_Owner->SetActive(false);
            };
        }

    private:
        bool m_Collected = false;
    };

} // namespace NFSEngine
