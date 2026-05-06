#pragma once
#include <NFSEngine.h>
#include "InteractivePiano.hpp"
#include "Core/Physics/PhysicsSystem.hpp"

class PianoKeyTrigger : public NFSEngine::Component {
public:
    int KeyIndex = 0;
    InteractivePiano* MainPiano = nullptr;
    NFSEngine::GameObject* TargetPlayer = nullptr;

    float CooldownTime = 0.2f;
    float ReleaseDelay = 0.15f;

    PianoKeyTrigger(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "PianoKeyTrigger"; }

    void SetBasePosition(const glm::vec3& pos) { m_BasePosition = pos; }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!MainPiano || !TargetPlayer) return;

        if (m_CurrentCooldown > 0.0f) {
            m_CurrentCooldown -= deltaTime.GetSeconds();
        }

        bool isCurrentlyColliding = NFSEngine::PhysicsSystem::CheckCollision(GetOwner(), TargetPlayer).IsColliding;

        if (isCurrentlyColliding) {

            m_TimeSinceLastCollision = 0.0f;

            if (!m_IsPressed && m_CurrentCooldown <= 0.0f) {
                m_IsPressed = true;
                MainPiano->PlayKey(KeyIndex);

                auto transform = GetOwner()->GetTransform();
                glm::vec3 pos = m_BasePosition;
                pos.y -= 0.1f;
                transform->SetPosition(pos);
            }

        } else {
            m_TimeSinceLastCollision += deltaTime.GetSeconds();

            if (m_IsPressed && m_TimeSinceLastCollision > ReleaseDelay) {
                m_IsPressed = false;

                m_CurrentCooldown = CooldownTime;

                auto transform = GetOwner()->GetTransform();
                transform->SetPosition(m_BasePosition);
            }
        }
    }

private:
    bool m_IsPressed = false;
    glm::vec3 m_BasePosition { 0.0f };

    float m_CurrentCooldown = 0.0f;
    float m_TimeSinceLastCollision = 0.0f;
};