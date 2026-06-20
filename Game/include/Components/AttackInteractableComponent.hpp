#pragma once

#include <NFSEngine.h>
#include <functional>

class AttackInteractableComponent : public NFSEngine::Component {
public:
    AttackInteractableComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "AttackInteractableComponent"; }

    std::function<void()> OnHit = nullptr;
};