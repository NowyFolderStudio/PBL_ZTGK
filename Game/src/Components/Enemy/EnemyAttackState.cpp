#include "Components/Enemy/EnemyAttackState.hpp"
#include "Components/Enemy/BasicEnemy.hpp"

void EnemyAttackState::Enter(BasicEnemy* enemy) {
    enemy->StopMoving();
    m_Timer = 0.0f;
}

void EnemyAttackState::Update(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) {
    m_Timer += deltaTime.GetSeconds();

    if (m_Timer >= m_AttackDuration) {
        enemy->ChangeState(enemy->StateChase);
    }
}

void EnemyAttackState::FixedUpdate(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) { }
void EnemyAttackState::Exit(BasicEnemy* enemy) { }