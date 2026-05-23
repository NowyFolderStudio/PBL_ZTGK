#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Components/CharacterController.hpp"

class CheckpointComponent : public NFSEngine::Component {
public:
    explicit CheckpointComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "CheckpointComponent"; }

    glm::vec3 SpawnOffset = glm::vec3(0.0f);

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            auto* cc = other->GetComponent<CharacterController>();
            if (!cc) return;
            cc->SpawnPosition = m_Owner->GetTransform()->GetPosition() + SpawnOffset;
        };
    }
};
