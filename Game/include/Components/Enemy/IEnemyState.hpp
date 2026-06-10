#pragma once

#include <NFSEngine.h>

class BasicEnemy;

class IEnemyState {
public:
    virtual ~IEnemyState() = default;

    virtual void Enter(BasicEnemy* enemy) = 0;

    virtual void Update(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) = 0;

    virtual void FixedUpdate(BasicEnemy* enemy, NFSEngine::DeltaTime deltaTime) = 0;

    virtual void Exit(BasicEnemy* enemy) = 0;
};