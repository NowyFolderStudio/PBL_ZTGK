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

    float TriggerThickness = 0.2f;

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

        triggerCol.Size = physicalCollider->Size * (glm::vec3(1.0f) - absDir) + (absDir * TriggerThickness);

        triggerCol.Offset = physicalCollider->Offset + (PushDirection * (physicalCollider->Size * 0.5f))
            + (PushDirection * (TriggerThickness * 0.5f));

        triggerCol.OnTriggerStay = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                auto* rb = other->GetComponent<NFSEngine::RigidBody3DComponent>();
                if (rb) {
                    glm::quat wallRotation = m_Owner->GetTransform()->GetWorldRotation();
                    glm::vec3 worldPushDirection = wallRotation * PushDirection;

                    if (worldPushDirection.x != 0.0f) rb->Velocity.x = worldPushDirection.x * PushSpeed;
                    if (worldPushDirection.z != 0.0f) rb->Velocity.z = worldPushDirection.z * PushSpeed;
                }
            }
        };
    }
};