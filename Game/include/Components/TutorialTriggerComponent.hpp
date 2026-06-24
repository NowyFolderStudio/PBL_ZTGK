#pragma once

#include <NFSEngine.h>
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "Managers/TutorialManager.hpp"

class TutorialTriggerComponent : public NFSEngine::Component {
public:
    explicit TutorialTriggerComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "TutorialTriggerComponent"; }

    TutorialPanel PanelToDisplay = TutorialPanel::Walk;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;

        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            if (TutorialManager::Instance) {
                TutorialManager::Instance->ShowTutorial(PanelToDisplay);
            }
        };

        collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            if (TutorialManager::Instance) {
                TutorialManager::Instance->HideTutorial();
            }
        };
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }
};