#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Components/CharacterController.hpp"

class CheckpointComponent : public NFSEngine::Component {
public:
    explicit CheckpointComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "CheckpointComponent"; }

    glm::vec3 SpawnOffset = glm::vec3(0.0f);
    float SpawnOffsetCorrectionX = -7.0;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Activated) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            auto* cc = other->GetComponent<CharacterController>();
            if (!cc) return;

            m_Activated = true;

            glm::vec3 checkpointWorldPos = m_Owner->GetTransform()->GetWorldPosition();
            glm::vec3 correctedOffset = SpawnOffset;
            correctedOffset.x += SpawnOffsetCorrectionX;
            glm::vec4 spawnPos = m_Owner->GetTransform()->GetGlobalMatrix() * glm::vec4(correctedOffset, 1.0f);
            cc->SpawnPosition = glm::vec3(spawnPos);

            m_Owner->SetActive(false);
        };
    }

private:
    bool m_Activated = false;
};
