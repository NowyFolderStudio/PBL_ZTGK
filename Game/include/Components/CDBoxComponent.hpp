#pragma once

#include <NFSEngine.h>
#include "Components/RotatingPlatform.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/AttackInteractableComponent.hpp"

class CDBoxComponent : public NFSEngine::Component {
public:
    CDBoxComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "CDBoxComponent"; }

    NFSEngine::GameObject* TargetButton = nullptr;
    NFSEngine::GameObject* TargetCD = nullptr;

    bool IsPlaying = true;

    glm::vec3 ButtonPressOffset = glm::vec3(0.0f, -0.2f, 0.0f);
    glm::vec3 EjectOffset = glm::vec3(-15.0f, 0.0f, 0.0f);
    float EjectDuration = 0.5f;

protected:
    void OnAwake() override {}

    void OnStart() override {
        size_t childCount = GetOwner()->GetTransform()->GetChildCount();
        for (size_t i = 0; i < childCount; i++) {
            auto* childTransform = GetOwner()->GetTransform()->GetChild(i);
            NFSEngine::GameObject* childGo = childTransform->GetOwner();

            std::string name = childGo->name;

            if (name.find("SpinningCD") != std::string::npos) {
                TargetCD = childGo;
            }
            else if (name.find("Button") != std::string::npos) {
                TargetButton = childGo;
            }
        }

        if (TargetCD) {
            m_InitialPosition = TargetCD->GetTransform()->GetPosition();

            auto* rotPlatform = TargetCD->GetComponent<RotatingPlatform>();
            if (!rotPlatform) {
                rotPlatform = &TargetCD->AddComponent<RotatingPlatform>();
            }
            rotPlatform->RotationSpeed = glm::vec3(0.0f, 90.0f, 0.0f);

            auto* cdCollider = TargetCD->GetComponent<NFSEngine::CylinderCollider3DComponent>();
            if (!cdCollider) {
                cdCollider = &TargetCD->AddComponent<NFSEngine::CylinderCollider3DComponent>();
                cdCollider->Height = 0.2f;
            }
        }

        if (TargetButton) {
            auto* btnCollider = TargetButton->GetComponent<NFSEngine::BoxCollider3DComponent>();
            if (!btnCollider) {
                btnCollider = &TargetButton->AddComponent<NFSEngine::BoxCollider3DComponent>();
            }
            btnCollider->IsTrigger = true;

            auto* btnRb = TargetButton->GetComponent<NFSEngine::RigidBody3DComponent>();
            if (!btnRb) {
                btnRb = &TargetButton->AddComponent<NFSEngine::RigidBody3DComponent>();
            }
            btnRb->IsKinematic = true;
            btnRb->UseGravity = false;

            auto* interactable = TargetButton->GetComponent<AttackInteractableComponent>();
            if (!interactable) {
                interactable = &TargetButton->AddComponent<AttackInteractableComponent>();
            }

            interactable->OnHit = [this]() {
                OnButtonPressed();
            };
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override {}

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_IsEjecting && TargetCD) {
            m_EjectTimer += deltaTime;
            float progress = m_EjectTimer / EjectDuration;

            if (progress >= 1.0f) {
                progress = 1.0f;
                m_IsEjecting = false;
            }

            glm::vec3 currentPos = glm::mix(m_InitialPosition, m_InitialPosition + EjectOffset, progress);
            TargetCD->GetTransform()->SetPosition(currentPos);
        }
    }

    void OnEnable() override {}
    void OnDisable() override {}

private:
    bool m_IsEjected = false;
    bool m_IsEjecting = false;
    float m_EjectTimer = 0.0f;
    glm::vec3 m_InitialPosition = glm::vec3(0.0f);

    void OnButtonPressed() {
        if (m_IsEjected) return;

        std::cout << "halo!" << std::endl;

        IsPlaying = !IsPlaying;
        m_IsEjected = true;
        m_IsEjecting = true;
        m_EjectTimer = 0.0f;

        if (TargetButton) {
            auto tf = TargetButton->GetTransform();
            glm::vec3 currentPos = tf->GetPosition();

            currentPos -= ButtonPressOffset;
            tf->SetPosition(currentPos);
        }
    }
};