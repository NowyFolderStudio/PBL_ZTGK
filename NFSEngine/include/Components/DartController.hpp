#pragma once

#include <NFSEngine.h>
#include "Components/DartRainAttackManager.hpp"
#include "Components/PhysicsComponents.hpp"
#include <string>
#include <vector>

class DartController : public NFSEngine::Component {
public:
    DartController(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartController"; }

    void SetRainAttackActive(bool isActive) {
        if (m_RainAttack) {
            m_RainAttack->IsActive = isActive;
        }
    }

protected:
    void OnStart() override {
        m_RainAttack = &GetOwner()->AddComponent<DartRainAttackComponent>();

        m_RainAttack->IsActive = false;

        m_RainAttack->TargetTrack = "Bass";
        m_RainAttack->PlayerTransform = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);

        int childCount = GetOwner()->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; i++) {
            auto* childNode = GetOwner()->GetTransform()->GetChild(i)->GetOwner();
            std::string childName = childNode->name;

            if (childName.find("DartStart") != std::string::npos) {
                auto* col = childNode->GetComponent<NFSEngine::BoxCollider3DComponent>();
                if (col) {
                    col->IsTrigger = true;
                    col->OnTriggerEnter = [this](NFSEngine::GameObject* otherObj) {
                        if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                            SetRainAttackActive(true);
                        }
                        };
                }
            }
            else if (childName.find("DartEnd") != std::string::npos) {
                auto* col = childNode->GetComponent<NFSEngine::BoxCollider3DComponent>();
                if (col) {
                    col->IsTrigger = true;
                    col->OnTriggerEnter = [this](NFSEngine::GameObject* otherObj) {
                        if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                            SetRainAttackActive(false);
                        }
                        };
                }
            }
        }
    }

public:
    void OnEvent(NFSEngine::Event& e) {
        if (m_RainAttack) {
            m_RainAttack->OnEvent(e);
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {}

private:
    DartRainAttackComponent* m_RainAttack = nullptr;
};