#pragma once
#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Components/Managers/DialogueManager.hpp"
#include <string>

#include <NFSEngine.h>

class DialogueTriggerComponent : public NFSEngine::Component {
public:
    DialogueTriggerComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "DialogueTriggerComponent"; }

    std::string SpeakerName = "Cat";
    std::string Message = "Look at that jump!";
    std::string PortraitPath = "assets/textures/cat.png";
    float Duration = 5.0f;
    bool TriggerOnce = true;

protected:
    void OnAwake() override { }

    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;

        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_HasTriggered && TriggerOnce) return;

            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            NFSEngine::DialogueManager::Get().ShowMessage(SpeakerName, Message, PortraitPath, Duration);
            m_HasTriggered = true;
        };
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnEnable() override { }

    void OnDisable() override { }

private:
    bool m_HasTriggered = false;
};
