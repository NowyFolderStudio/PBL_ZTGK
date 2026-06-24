#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"
#include "Components/CharacterController.hpp"
#include "Core/Log.hpp"
#include <vector>

class CheckpointComponent : public NFSEngine::Component {
public:
    explicit CheckpointComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~CheckpointComponent() {
        if (s_ActiveCheckpoint == this) {
            s_ActiveCheckpoint = nullptr;
        }
    }

    std::string GetName() const override { return "CheckpointComponent"; }

    glm::vec3 SpawnOffset = glm::vec3(0.0f);
    float SpawnOffsetCorrectionX = -7.0f;

    inline static CheckpointComponent* s_ActiveCheckpoint = nullptr;

private:
    bool m_IsActive = false;
    float m_CurrentActiveVisual = 0.0f;
    NFSEngine::ModelComponent* m_VisualModel = nullptr;

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
                m_IsActive = true;

                NFS_CORE_INFO("Checkpoint {0} został aktywowany!", m_Owner->name);
            }
        };
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_VisualModel) {
            std::vector<NFSEngine::Transform*> nodesToCheck;
            nodesToCheck.push_back(m_Owner->GetTransform());

            for (size_t i = 0; i < nodesToCheck.size(); ++i) {
                auto* currentTransform = nodesToCheck[i];

                if (auto* modelComp = currentTransform->GetOwner()->GetComponent<NFSEngine::ModelComponent>()) {
                    m_VisualModel = modelComp;
                    break;
                }

                int childCount = currentTransform->GetChildCount();
                for (int j = 0; j < childCount; ++j) {
                    nodesToCheck.push_back(currentTransform->GetChild(j));
                }
            }
        }

        if (m_VisualModel && m_VisualModel->GetMaterial(0)) {
            float targetVisual = m_IsActive ? 1.0f : 0.0f;
            m_CurrentActiveVisual = glm::mix(m_CurrentActiveVisual, targetVisual, deltaTime.GetSeconds() * 3.0f);

            m_VisualModel->GetMaterial(0)->SetFloat("u_IsActive", m_CurrentActiveVisual);
        }
    }

public:
    void Deactivate() { m_IsActive = false; }
};