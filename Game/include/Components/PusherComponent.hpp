#pragma once
#include <NFSEngine.h>
#include "Components/PhysicsComponents.hpp"

class PusherComponent : public NFSEngine::Component {
public:
    NFSEngine::Scene* CurrentScene = nullptr;

    explicit PusherComponent(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "PusherComponent"; }

    glm::vec3 PushDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float PushSpeed = 10.0f;

    float TriggerThickness = 0.8f;

protected:
    void OnStart() override {
        auto* physicalCollider = m_Owner->GetComponent<NFSEngine::BoxCollider3DComponent>();

        if (!physicalCollider || !CurrentScene) {
            return;
        }

        std::string triggerName = m_Owner->name + "_TriggerZderzak";
        NFSEngine::GameObject* triggerObj = CurrentScene->CreateGameObject(triggerName);

        triggerObj->GetTransform()->SetParent(m_Owner->GetTransform(), false);
        triggerObj->GetTransform()->SetPosition(glm::vec3(0.0f));
        triggerObj->GetTransform()->SetRotation(glm::vec3(0.0f));
        triggerObj->GetTransform()->SetScale(glm::vec3(1.0f));

        auto& triggerCol = triggerObj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        triggerCol.IsTrigger = true;

        glm::vec3 absDir = glm::abs(PushDirection);

        triggerCol.Size = physicalCollider->Size + (absDir * TriggerThickness);

        triggerCol.Offset = physicalCollider->Offset + (PushDirection * (TriggerThickness * 0.5f));

        triggerCol.OnTriggerStay = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                auto* rb = other->GetComponent<NFSEngine::RigidBody3DComponent>();
                if (rb) {
                    if (PushDirection.x != 0.0f) rb->Velocity.x = PushDirection.x * PushSpeed;
                    if (PushDirection.z != 0.0f) rb->Velocity.z = PushDirection.z * PushSpeed;
                }
            }
        };
    }
};