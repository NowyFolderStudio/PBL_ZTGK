#pragma once

#include <NFSEngine.h>
#include <glm/gtx/quaternion.hpp>
#include "Components/Managers/LivesManager.hpp"

class FallingDartComponent : public NFSEngine::Component {
public:
    FallingDartComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "FallingDartComponent"; }

    float FallSpeed = 200.0f;
    float TipOffset = 0.0f;
    float StayDuration = 2.0f;
    bool m_ColliderHooked = false;

    void Fire(glm::vec3 targetPos, NFSEngine::GameObject* indicator) {
        m_TargetPosition = targetPos;
        m_LinkedIndicator = indicator;
        m_IsFalling = true;
        m_DespawnTimer = 0.0f;
        m_HasDealtDamage = false;

        if (!m_ColliderHooked) {
            if (auto* boxCol = GetOwner()->GetComponent<NFSEngine::BoxCollider3DComponent>()) {
                boxCol->OnTriggerEnter = [this](NFSEngine::GameObject* otherObj) {

                    if (m_IsFalling && !m_HasDealtDamage && otherObj->CompareTag(NFSEngine::Tags::Player)) {
                        if (LivesManager::Instance) {
                            LivesManager::Instance->LoseHeart();
                            m_HasDealtDamage = true;
                        }
                    }
                    };
                m_ColliderHooked = true;
            }
        }

        glm::vec3 myPos = GetOwner()->GetTransform()->GetPosition();
        m_FlightDirection = glm::normalize(m_TargetPosition - myPos);

        glm::quat lookRot = glm::quatLookAt(m_FlightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        GetOwner()->GetTransform()->SetRotation(lookRot);
    }

protected:
    void OnStart() override {}

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_IsFalling) {
            auto* transform = GetOwner()->GetTransform();
            glm::vec3 currentPos = transform->GetPosition();

            currentPos += m_FlightDirection * FallSpeed * deltaTime.GetSeconds();
            transform->SetPosition(currentPos);

            glm::vec3 upVector = transform->GetRotation() * glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 currentTipPosition = currentPos - (upVector * TipOffset);

            if (currentTipPosition.y <= m_TargetPosition.y) {
                m_IsFalling = false;
                transform->SetPosition(m_TargetPosition + (upVector * TipOffset));
            }
        }
        else {
            m_DespawnTimer += deltaTime.GetSeconds();
            if (m_DespawnTimer >= StayDuration) {
                GetOwner()->Destroy();
            }
        }
    }

private:
    glm::vec3 m_TargetPosition;
    glm::vec3 m_FlightDirection;
    bool m_IsFalling = false;
    bool m_HasDealtDamage = false;
    float m_DespawnTimer = 0.0f;
    NFSEngine::GameObject* m_LinkedIndicator = nullptr;
};