#pragma once

#include "Components/ActivatebleComponent.hpp"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

class Doors : public ActivatebleComponent {
public:
    Doors(NFSEngine::GameObject* owner)
        : ActivatebleComponent(owner) { }

    std::string GetName() const override { return "Doors"; }

    void Open() { m_DoorOpen = true; }

    glm::vec3 OpenVectorRotation = { 0.0f, -135.0f, 0.0f };
    float AnimationSpeed = 5.0f;

protected:
    void OnStart() override {
        if (m_HingeTransform == nullptr) {
            m_HingeTransform = m_Owner->GetTransform()->GetChild(0)->GetOwner()->GetTransform();
        }
        if (m_HingeTransform != nullptr) {
            m_ClosedVectorRotation = glm::degrees(glm::eulerAngles(m_HingeTransform->GetRotation()));
            m_CurrentVectorRotation = m_ClosedVectorRotation;
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_HingeTransform == nullptr) return;

        glm::vec3 targetRotation = m_DoorOpen ? OpenVectorRotation : m_ClosedVectorRotation;

        float lerpSpeed = AnimationSpeed * (float)deltaTime;

        m_CurrentVectorRotation += (targetRotation - m_CurrentVectorRotation) * lerpSpeed;

        m_HingeTransform->SetRotation(m_CurrentVectorRotation);
    }

private:
    NFSEngine::Transform* m_HingeTransform = nullptr;
    bool m_DoorOpen = false;

    glm::vec3 m_CurrentVectorRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_ClosedVectorRotation = { 0.0f, 0.0f, 0.0f };
};