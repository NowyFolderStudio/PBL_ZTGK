#pragma once

#include <NFSEngine.h>
#include <memory>

enum class TutorialPanel {
    Walk,
    LookAround,
    Jump,
    Pause,
    Checkpoint,
    WallJump,
    Aura,
    DoubleJump,
    CDPlayer,
    Attack
};

class TutorialManager : public NFSEngine::Component {
public:
    inline static TutorialManager* Instance = nullptr;

    TutorialManager(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    ~TutorialManager() override {
        if (Instance == this) {
            Instance = nullptr;
        }
    }

    std::string GetName() const override { return "TutorialManager"; }

    NFSEngine::Action<TutorialPanel> OnShowTutorial;
    NFSEngine::Action<> OnHideTutorial;

    void ShowTutorial(TutorialPanel panel) { OnShowTutorial.Invoke(panel); }

    void HideTutorial() { OnHideTutorial.Invoke(); }

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
};