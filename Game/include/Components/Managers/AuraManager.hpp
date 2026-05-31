#pragma once

#include <NFSEngine.h>

enum class AuraType { // TODO: Rename it
    First,
    Second,
    Third
};

class AuraManager : public NFSEngine::Component {
public:
    inline static AuraManager* Instance = nullptr;

    AuraManager(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~AuraManager() override {
        if (Instance == this) {
            Instance = nullptr;
        }
    }

    std::string GetName() const override { return "AuraManager"; }

    NFSEngine::Action<AuraType> OnAuraChanged;
    AuraType CurrentAura = AuraType::First;

protected:
    void OnAwake() override {
        if (Instance != nullptr && Instance != this) {
            return;
        }
        Instance = this;
    }

    void OnStart() override { }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnEnable() override { }

    void OnDisable() override { }

public:
    void ChangeAura(AuraType newAura) {
        if (CurrentAura == newAura) return;

        CurrentAura = newAura;

        OnAuraChanged.Invoke(newAura);
    }
};