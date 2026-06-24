#pragma once
#include <NFSEngine.h>

class InteractivePiano;

class PianoKeyTrigger : public NFSEngine::Component {
public:
    int KeyIndex = 0;
    InteractivePiano* MainPiano = nullptr;
    NFSEngine::GameObject* TargetPlayer = nullptr;

    float CooldownTime = 0.2f;
    float ReleaseDelay = 0.15f;

    PianoKeyTrigger(NFSEngine::GameObject* owner)
        : Component(owner) {
    }

    std::string GetName() const override { return "PianoKeyTrigger"; }

    void SetBasePosition(const glm::vec3& pos) { m_BasePosition = pos; }

protected:
    void OnStart() override {
        auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();

        if (collider) {
            collider->IsTrigger = true;

            collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (!TargetPlayer || other == TargetPlayer) {
                    m_IsCurrentlyColliding = true;
                }
                };

            collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
                if (!TargetPlayer || other == TargetPlayer) {
                    m_IsCurrentlyColliding = false;
                }
                };
        }
    }

public:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;

private:
    bool m_IsPressed = false;
    bool m_IsCurrentlyColliding = false;
    glm::vec3 m_BasePosition{ 0.0f };

    float m_CurrentCooldown = 0.0f;
    float m_TimeSinceLastCollision = 0.0f;
};