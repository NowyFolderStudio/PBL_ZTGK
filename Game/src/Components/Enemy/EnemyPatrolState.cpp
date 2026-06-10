#include "Components/Enemy/EnemyPatrolState.hpp"
#include "Components/Enemy/BasicEnemy.hpp"

void EnemyPatrolState::Enter(BasicEnemy* enemy) { }

void EnemyPatrolState::Update(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) { }

void EnemyPatrolState::FixedUpdate(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) {
    glm::vec3 myPos = enemy->GetOwner()->GetTransform()->GetWorldPosition();
    glm::vec3 playerPos = enemy->GetPlayerPosition();

    glm::vec3 myPosFlat = glm::vec3(myPos.x, 0.0f, myPos.z);
    glm::vec3 playerPosFlat = glm::vec3(playerPos.x, 0.0f, playerPos.z);

    if (glm::distance(myPosFlat, playerPosFlat) <= enemy->DetectionRadius) {
        enemy->ChangeState(enemy->StateChase);
        return;
    }

    glm::vec3 currentTarget = enemy->IsMovingToB() ? enemy->PatrolPointB : enemy->PatrolPointA;
    glm::vec3 targetFlat = glm::vec3(currentTarget.x, 0.0f, currentTarget.z);

    if (glm::distance(myPosFlat, targetFlat) < 0.4f) {
        enemy->TogglePatrolDirection();
    }

    enemy->MoveTowards(targetFlat, enemy->MovementSpeed);
}

void EnemyPatrolState::Exit(BasicEnemy* enemy) { }
