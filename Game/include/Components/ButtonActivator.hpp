#pragma once
#include "Components/ActivatebleComponent.hpp"
#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Core/Tags.hpp"

class ButtonActivator : public NFSEngine::Component {
public:
    ButtonActivator(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "ButtonActivator"; }

    void SetActivatedObject(NFSEngine::GameObject* obj) { m_ActivatedObject = obj; }

    NFSEngine::GameObject* GetActivatedObject() const { return m_ActivatedObject; }

protected:
    void OnStart() override {
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
        m_Activateble = m_ActivatedObject->GetComponent<ActivatebleComponent>();
        m_Collider->IsTrigger = true;
        m_Collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                m_Activateble->OnActivationEnter();
            }
        };
        m_Collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                m_Activateble->OnActivationExit();
            }
        };
        m_Collider->OnTriggerStay = [this](NFSEngine::GameObject* other) {
            if (other->CompareTag(NFSEngine::Tags::Player)) {
                m_Activateble->OnActivationStay();
            }
        };
    }
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override { }

private:
    NFSEngine::GameObject* m_ActivatedObject = nullptr;
    ActivatebleComponent* m_Activateble = nullptr;
    NFSEngine::ColliderComponent* m_Collider = nullptr;
};