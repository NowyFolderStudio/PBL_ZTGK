#pragma once
#include "IEnemyState.hpp"
#include <NFSEngine.h>

class BasicEnemy;

class EnemyPatrolState : public IEnemyState {
public:
    void Enter(BasicEnemy* enemy) override;

    void Update(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) override;

    void FixedUpdate(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) override;

    void Exit(BasicEnemy* enemy) override;
};