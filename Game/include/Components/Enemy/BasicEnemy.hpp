#pragma once
#include <NFSEngine.h>
#include "Components/Managers/LivesManager.hpp"
#include "IEnemyState.hpp"
#include "EnemyPatrolState.hpp"
#include "EnemyChaseState.hpp"
#include "EnemyAttackState.hpp"
#include "Components/CharacterController.hpp"

class BasicEnemy : public NFSEngine::Component {
public:
    BasicEnemy(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) {
        StatePatrol = new EnemyPatrolState();
        StateChase = new EnemyChaseState();
        StateAttack = new EnemyAttackState();
    }

    ~BasicEnemy() {
        delete StatePatrol;
        delete StateChase;
        delete StateAttack;
    }

    std::string GetName() const override { return "BasicEnemy"; }

    glm::vec3 PatrolPointA = glm::vec3(0.0f);
    glm::vec3 PatrolPointB = glm::vec3(0.0f);

    float MovementSpeed = 3.0f;
    float ChaseSpeed = 5.0f;
    float DetectionRadius = 5.0f;
    float DamageCooldown = 2.0f;
    float KnockBackStrength = 25.0f;

    IEnemyState* StatePatrol = nullptr;
    IEnemyState* StateChase = nullptr;
    IEnemyState* StateAttack = nullptr;

    void ChangeState(IEnemyState* newState) {
        if (m_CurrentState != nullptr) {
            m_CurrentState->Exit(this);
        }
        m_CurrentState = newState;
        if (m_CurrentState != nullptr) {
            m_CurrentState->Enter(this);
        }
    }

    glm::vec3 GetPlayerPosition() const { return m_Player ? m_Player->GetTransform()->GetWorldPosition() : glm::vec3(0.0f); }
    glm::vec3 GetPosition() const { return GetOwner()->GetTransform()->GetWorldPosition(); }
    NFSEngine::GameObject* GetPlayer() const { return m_Player; }

    void StopMoving() {
        if (m_RigidBody) {
            m_RigidBody->Velocity = glm::vec3(0.0f);
        }
    }

    bool IsMovingToB() const { return m_MovingToB; }
    void TogglePatrolDirection() { m_MovingToB = !m_MovingToB; }

    void MoveTowards(glm::vec3 targetPosFlat, float speed) {
        glm::vec3 myPos = GetOwner()->GetTransform()->GetWorldPosition();
        glm::vec3 myPosFlat = glm::vec3(myPos.x, 0.0f, myPos.z);
        glm::vec3 direction = targetPosFlat - myPosFlat;

        if (glm::length(direction) > 0.01f) {
            direction = glm::normalize(direction);
            m_RigidBody->Velocity.x = direction.x * speed;
            m_RigidBody->Velocity.z = direction.z * speed;

            float targetAngle = atan2(direction.x, direction.z);
            GetOwner()->GetTransform()->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));
        }
    }

    void DealDamage(NFSEngine::GameObject* other, glm::vec3 contactNormal) {
        if (other == m_Player && m_LastDamageTime <= 0.0f) {
            if (auto* playerController = m_Player->GetComponent<CharacterController>()) {
                glm::vec3 myPos = GetOwner()->GetTransform()->GetWorldPosition();
                bool damageDealt = playerController->TakeDamage(myPos, KnockBackStrength);

                if (damageDealt) {
                    m_LastDamageTime = DamageCooldown;

                    ChangeState(StateAttack);
                }
            }
        }
    }

protected:
    NFSEngine::RigidBody3DComponent* m_RigidBody = nullptr;
    NFSEngine::GameObject* m_Player = nullptr;
    NFSEngine::ColliderComponent* m_Collider = nullptr;
    IEnemyState* m_CurrentState = nullptr;

    bool m_MovingToB = true;
    float m_LastDamageTime = 0.0f;

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

        ChangeState(StatePatrol);
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_Player || !m_RigidBody) return;
        if (m_CurrentState) m_CurrentState->FixedUpdate(this, deltaTime);
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_LastDamageTime > 0.0f) {
            m_LastDamageTime -= deltaTime.GetSeconds();
        }
        if (m_CurrentState) m_CurrentState->Update(this, deltaTime);
    }
};