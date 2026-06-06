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
            m_Collider->OnCollisionEnter = [this](NFSEngine::GameObject* other, glm::vec3 contactNormal) {
                if (other == m_Player && m_LastDamageTime <= 0.0f) {
                    if (LivesManager::Instance) {
                        LivesManager::Instance->LoseHeart();
                        m_LastDamageTime = DamageCooldown;
                    }
                }
            };
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_Player || !m_RigidBody) return;

		glm::vec3 myPos = GetOwner()->GetTransform()->GetPosition();
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_LastDamageTime > 0.0f) {
            m_LastDamageTime -= deltaTime.GetSeconds();
        }
    }

    void OnEnable() override { }

    void OnDisable() override { }
};
