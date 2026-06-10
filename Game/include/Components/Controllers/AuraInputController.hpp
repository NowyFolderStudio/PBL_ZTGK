#pragma once
#include <NFSEngine.h>
#include "InputActions.hpp"
#include "../Aura/AuraManager.hpp"

class AuraInputController : public NFSEngine::Component {
public:
    AuraInputController(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "AuraInputController"; }

protected:
    void OnAwake() override { }
    void OnStart() override { }
    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!AuraManager::Instance) return;

        if (NFSEngine::InputActionManager::IsDown("SelectFirstAura")) {
            AuraManager::Instance->ChangeAura(AuraType::First);
        } else if (NFSEngine::InputActionManager::IsDown("SelectSecondAura")) {
            AuraManager::Instance->ChangeAura(AuraType::Second);
        }
    }

    void OnEnable() override { }
    void OnDisable() override { }
};