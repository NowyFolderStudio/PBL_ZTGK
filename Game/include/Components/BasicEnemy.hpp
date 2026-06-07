#pragma once

#include <NFSEngine.h>

#include "Managers/LivesManager.hpp"

class BasicEnemy : public NFSEngine::Component {
public:
    BasicEnemy(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "BasicEnemy"; }

    glm::vec3 PatrolPointA = glm::vec3(0.0f);
    glm::vec3 PatrolPointB = glm::vec3(0.0f);

    float MovementSpeed = 3.0f;
    float ChaseSpeed = 5.0f;

    float DetectionRadius = 5.0f;

    float DamageCooldown = 2.0f;

    float KnockBackStrength = 25.0f;

protected:
    NFSEngine::RigidBody3DComponent* m_RigidBody = nullptr;
    NFSEngine::GameObject* m_Player = nullptr;
    NFSEngine::ColliderComponent* m_Collider = nullptr;

    bool m_MovingToB = true;
    float m_LastDamageTime = 0.0f;

    void OnAwake() override { }

    void OnStart() override {
        m_RigidBody = GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();

        const auto& gameObjects = GetOwner()->GetScene()->GetAllGameObjects();
        for (const auto& go : gameObjects) {
            if (go->CompareTag(NFSEngine::Tags::Player)) {
                m_Player = go.get();
                break;
            }
        }

        if (m_Collider) {
            m_Collider->OnCollisionEnter
                = [this](NFSEngine::GameObject* other, glm::vec3 contactNormal) { DealDamage(other, contactNormal); };
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_Player || !m_RigidBody) return;

        glm::vec3 myPos = GetOwner()->GetTransform()->GetWorldPosition();
        glm::vec3 playerPos = m_Player->GetTransform()->GetWorldPosition();

        glm::vec3 myPosFlat = glm::vec3(myPos.x, 0.0f, myPos.z);
        glm::vec3 playerPosFlat = glm::vec3(playerPos.x, 0.0f, playerPos.z);

        float distanceToPlayer = glm::distance(myPosFlat, playerPosFlat);

        glm::vec3 targetPosFlat;
        float currentSpeed;

        if (distanceToPlayer <= DetectionRadius) {
            targetPosFlat = playerPosFlat;
            currentSpeed = ChaseSpeed;
        } else {
            glm::vec3 currentTarget = m_MovingToB ? PatrolPointB : PatrolPointA;
            glm::vec3 targetFlat = glm::vec3(currentTarget.x, 0.0f, currentTarget.z);

            if (glm::distance(myPosFlat, targetFlat) < 0.4f) {
                m_MovingToB = !m_MovingToB;
            }

            targetPosFlat = targetFlat;
            currentSpeed = MovementSpeed;
        }

        glm::vec3 direction = targetPosFlat - myPosFlat;
        if (glm::length(direction) > 0.01f) {
            direction = glm::normalize(direction);

            m_RigidBody->Velocity.x = direction.x * currentSpeed;
            m_RigidBody->Velocity.z = direction.z * currentSpeed;

            float targetAngle = atan2(direction.x, direction.z);
            GetOwner()->GetTransform()->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_LastDamageTime > 0.0f) {
            m_LastDamageTime -= deltaTime.GetSeconds();
        }
    }

    void OnEnable() override { }

    void OnDisable() override { }

    void DealDamage(NFSEngine::GameObject* other, glm::vec3 contactNormal) {
        if (other == m_Player && m_LastDamageTime <= 0.0f) {
            if (auto* playerController = m_Player->GetComponent<CharacterController>()) {
                glm::vec3 myPos = GetOwner()->GetTransform()->GetWorldPosition();

                bool damageDealt = playerController->TakeDamage(myPos, KnockBackStrength);

                if (damageDealt) {
                    m_LastDamageTime = DamageCooldown;
                }
            }
        }
    }

    void TakeDamage() {
        // TODO: Add health and damage logic for the enemy
    }
};
