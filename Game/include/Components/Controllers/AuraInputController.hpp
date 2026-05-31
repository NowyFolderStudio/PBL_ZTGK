#pragma once
#include <NFSEngine.h>
#include "InputActions.hpp"
#include "../Managers/AuraManager.hpp"

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

        if (NFSEngine::InputActionManager::IsDown("NextAura")) {
            CycleAura(true);
        } else if (NFSEngine::InputActionManager::IsDown("PreviousAura")) {
            CycleAura(false);
        }
    }

    void OnEnable() override { }
    void OnDisable() override { }

private:
    void CycleAura(bool forward) {
        AuraType current = AuraManager::Instance->CurrentAura;
        AuraType nextAura = current;

        if (forward) {
            switch (current) {
            case AuraType::First:
                nextAura = AuraType::Second;
                break;
            case AuraType::Second:
                nextAura = AuraType::Third;
                break;
            case AuraType::Third:
                nextAura = AuraType::First;
                break;
            default:
                nextAura = AuraType::First;
                break;
            }
        } else {
            switch (current) {
            case AuraType::First:
                nextAura = AuraType::Third;
                break;
            case AuraType::Second:
                nextAura = AuraType::First;
                break;
            case AuraType::Third:
                nextAura = AuraType::Second;
                break;
            default:
                nextAura = AuraType::First;
                break;
            }
        }

        AuraManager::Instance->ChangeAura(nextAura);
    }
};