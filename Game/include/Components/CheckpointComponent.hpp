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
    float SpawnOffsetCorrectionX = -7.0;

    inline static CheckpointComponent* s_ActiveCheckpoint = nullptr;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            auto* cc = other->GetComponent<CharacterController>();
            if (!cc) return;

            glm::vec3 checkpointWorldPos = m_Owner->GetTransform()->GetWorldPosition();
            glm::vec3 correctedOffset = SpawnOffset;
            glm::vec4 spawnPos = m_Owner->GetTransform()->GetGlobalMatrix() * glm::vec4(correctedOffset, 1.0f);
            cc->SpawnPosition = glm::vec3(spawnPos);

            if (s_ActiveCheckpoint != this) {

                if (s_ActiveCheckpoint != nullptr) {
                    s_ActiveCheckpoint->Deactivate();
                }

                s_ActiveCheckpoint = this;

                // Here we can add some visual or audio feedback to indicate that the checkpoint has been activated

                NFS_CORE_INFO("Checkpoint {0} has been activated!", m_Owner->name);
            }
        };
    }

public:
    void Deactivate() {
        // Here we can add some visual or audio feedback to indicate that the checkpoint has been deactivated
    }
};