#pragma once
#include "Components/ActivatebleComponent.hpp"
#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Core/Tags.hpp"
#include <glm/ext/vector_float3.hpp>

class ButtonActivator : public NFSEngine::Component {
public:
    ButtonActivator(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "ButtonActivator"; }

    void SetActivatedObject(NFSEngine::GameObject* obj) { m_ActivatedObject = obj; }
    NFSEngine::GameObject* GetActivatedObject() const { return m_ActivatedObject; }

    float CooldownTime = 0.2f;
    float ReleaseDelay = 0.15f;

protected:
    void OnAwake() override {
        m_Transform = GetOwner()->GetTransform();
        if (m_Transform) {
            m_BasePosition = m_Transform->GetPosition();
        }
    }

    void OnStart() override {
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();

        if (!m_Collider) {
            NFS_CORE_WARN("ButtonActivator: Brak ColliderComponent na obiekcie!");
            return;
        }

        if (m_ActivatedObject) {
            m_Activateble = m_ActivatedObject->GetComponent<ActivatebleComponent>();
            if (!m_Activateble) {
                NFS_CORE_WARN("ButtonActivator: Przypisany obiekt nie posiada ActivatebleComponent!");
            }
        }

        m_Collider->IsTrigger = true;

        m_Collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                m_IsCurrentlyColliding = true;
            }
        };

        m_Collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                m_IsCurrentlyColliding = false;
            }
        };
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_CurrentCooldown > 0.0f) {
            m_CurrentCooldown -= deltaTime.GetSeconds();
        }

        if (m_IsCurrentlyColliding) {
            m_TimeSinceLastCollision = 0.0f;

            if (!m_IsPressed && m_CurrentCooldown <= 0.0f) {
                m_IsPressed = true;

                if (m_Transform) {
                    glm::vec3 pos = m_BasePosition;
                    pos.y -= 0.1f;
                    m_Transform->SetPosition(pos);
                }

                if (m_Activateble) {
                    m_Activateble->OnActivationEnter();
                }
            }

            if (m_IsPressed && m_Activateble) {
                m_Activateble->OnActivationStay();
            }

        } else {
            m_TimeSinceLastCollision += deltaTime.GetSeconds();

            if (m_IsPressed && m_TimeSinceLastCollision > ReleaseDelay) {
                m_IsPressed = false;
                m_CurrentCooldown = CooldownTime;

                if (m_Transform) {
                    m_Transform->SetPosition(m_BasePosition);
                }

                // Logika dezaktywacji
                if (m_Activateble) {
                    m_Activateble->OnActivationExit();
                }
            }
        }
    }

private:
    NFSEngine::GameObject* m_ActivatedObject = nullptr;
    ActivatebleComponent* m_Activateble = nullptr;
    NFSEngine::ColliderComponent* m_Collider = nullptr;
    NFSEngine::Transform* m_Transform = nullptr;

    bool m_IsPressed = false;
    bool m_IsCurrentlyColliding = false;
    glm::vec3 m_BasePosition { 0.0f };

    float m_CurrentCooldown = 0.0f;
    float m_TimeSinceLastCollision = 0.0f;
};