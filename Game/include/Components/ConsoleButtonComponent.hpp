#pragma once
#include <NFSEngine.h>
#include <glm/glm.hpp>
#include "Components/CubeMesh.hpp" 

class ConsoleButtonComponent : public NFSEngine::Component {
public:
    int ButtonIndex = 0;
    NFSEngine::GameObject* TargetPlayer = nullptr;

    glm::vec3 BaseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float BaseStrength = 0.8f;

    glm::vec3 ActiveColor = glm::vec3(1.0f, 0.0f, 0.0f);
    float ActiveStrength = 5.0f;

    ConsoleButtonComponent(NFSEngine::GameObject* owner)
        : Component(owner) {
    }

    std::string GetName() const override { return "ConsoleButtonComponent"; }

protected:
    void OnStart() override {
        auto renderComponent = GetOwner()->GetComponent<NFSEngine::CubeMesh>();
        if (renderComponent) {
            m_Material = renderComponent->GetMaterial();
            SetMaterialEmission(BaseColor, BaseStrength);
        }

        auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
        if (collider) {
            collider->IsTrigger = true;

            collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (!TargetPlayer || other == TargetPlayer) {
                    m_IsActive = true;
                    SetMaterialEmission(ActiveColor, ActiveStrength);

                    auto transform = GetOwner()->GetTransform();
                    glm::vec3 pos = transform->GetPosition();
                    pos.y -= 0.1f;
                    transform->SetPosition(pos);
                }
                };

            collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
                if (!TargetPlayer || other == TargetPlayer) {
                    m_IsActive = false;
                    SetMaterialEmission(BaseColor, BaseStrength);

                    auto transform = GetOwner()->GetTransform();
                    glm::vec3 pos = transform->GetPosition();
                    pos.y += 0.1f;
                    transform->SetPosition(pos);
                }
                };
        }
    }

private:
    std::shared_ptr<NFSEngine::Material> m_Material = nullptr;
    bool m_IsActive = false;

    void SetMaterialEmission(const glm::vec3& color, float strength) {
        if (m_Material) {
            m_Material->EmissiveColor = color;
            m_Material->EmissiveStrength = strength;
        }
    }
};