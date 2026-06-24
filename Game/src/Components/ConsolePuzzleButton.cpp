#include "Components/ConsolePuzzleButton.hpp"
#include "Components/ConsolePuzzle.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include <string>

void ConsolePuzzleButton::OnPlayerStepped() {
    if (puzzleManager != nullptr) {
        puzzleManager->PressButton(buttonID);
    }
}

void ConsolePuzzleButton::OnAwake() {
    m_Transform = GetOwner()->GetTransform();
    if (m_Transform) {
        m_BasePosition = m_Transform->GetPosition();
    }

    auto modelComp = GetOwner()->GetComponent<NFSEngine::ModelComponent>();
    if (modelComp) {
        m_Material = modelComp->GetMaterial(0);
    }
}

void ConsolePuzzleButton::OnStart() {
    ResetMaterialEmission();

    auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
    if (collider) {
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            m_IsCurrentlyColliding = true;
        };

        collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            m_IsCurrentlyColliding = false;
        };
    }
}

void ConsolePuzzleButton::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_CurrentCooldown > 0.0f) {
        m_CurrentCooldown -= deltaTime.GetSeconds();
    }

    if (m_IsCurrentlyColliding) {
        m_TimeSinceLastCollision = 0.0f;

        if (!m_IsPressed && m_CurrentCooldown <= 0.0f) {
            m_IsPressed = true;
            SetMaterialEmission(ActiveColor, ActiveStrength);

            if (m_Transform) {
                glm::vec3 pos = m_BasePosition;
                pos.y -= 0.1f;
                m_Transform->SetPosition(pos);
            }

            OnPlayerStepped();
        }
    } else {
        m_TimeSinceLastCollision += deltaTime.GetSeconds();

        if (m_IsPressed && m_TimeSinceLastCollision > ReleaseDelay) {
            m_IsPressed = false;
            m_CurrentCooldown = CooldownTime;

            ResetMaterialEmission();
            if (m_Transform) {
                m_Transform->SetPosition(m_BasePosition);
            }
        }
    }
}