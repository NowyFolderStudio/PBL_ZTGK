#include "Components/Enemy/EnemyChaseState.hpp"
#include "Components/Enemy/BasicEnemy.hpp"

void EnemyChaseState::Enter(BasicEnemy* enemy) { }

void EnemyChaseState::Update(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) { }

void EnemyChaseState::FixedUpdate(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) {
    glm::vec3 myPos = enemy->GetPosition();
    glm::vec3 playerPos = enemy->GetPlayerPosition();

    glm::vec3 myPosFlat = glm::vec3(myPos.x, 0.0f, myPos.z);
    glm::vec3 playerPosFlat = glm::vec3(playerPos.x, 0.0f, playerPos.z);

    if (glm::distance(myPosFlat, playerPosFlat) > enemy->DetectionRadius + 2.0f) {
        enemy->ChangeState(enemy->StatePatrol);
        return;
    }

    enemy->MoveTowards(playerPosFlat, enemy->ChaseSpeed);
}

void EnemyChaseState::Exit(BasicEnemy* enemy) { }