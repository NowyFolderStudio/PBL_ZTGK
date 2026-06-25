#pragma once

#include <NFSEngine.h>
#include <vector>
#include <string>
#include <iostream>

struct SliceAnimationData {
    NFSEngine::GameObject* TargetObject;
    glm::vec3 StartPosition;
    glm::vec3 EndPosition;
};

class DartStairsComponent : public NFSEngine::Component {
public:
    DartStairsComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartStairsComponent"; }

    std::vector<NFSEngine::GameObject*> AllSlices;
    float TransitionDuration = 2.0f;

    void StartTransition() {
        if (AllSlices.empty()) return;

        m_Timer = 0.0f;
        m_IsAnimating = true;

        NFS_CORE_INFO("DartStairsComponent: Rozpoczynam fazê schodów!");

        for (auto* slice : AllSlices) {
            NFSEngine::GameObject* collidersNode = FindChildByName(slice, "Colliders");
            if (!collidersNode) collidersNode = FindChildByName(slice, "colliders");

            if (collidersNode) {
                HookTriggersRecursive(collidersNode, slice->name);
            }
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
    }

private:
    bool m_IsAnimating = false;
    float m_Timer = 0.0f;

    NFSEngine::GameObject* FindChildByName(NFSEngine::GameObject* parent, const std::string& name) {
        int childCount = parent->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; ++i) {
            auto* child = parent->GetTransform()->GetChild(i)->GetOwner();
            if (child->name.find(name) != std::string::npos) {
                return child;
            }
        }
        return nullptr;
    }

    void HookTriggersRecursive(NFSEngine::GameObject* node, const std::string& parentSliceName) {
        if (auto* boxCol = node->GetComponent<NFSEngine::BoxCollider3DComponent>()) {
            boxCol->IsTrigger = true;
            boxCol->Size.y += 0.5f;

            boxCol->OnTriggerEnter = [parentSliceName](NFSEngine::GameObject* otherObj) {
                if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                    std::cout << "GRACZ WSZED£ NA PLASTER: " << parentSliceName << std::endl;
                }
                };
        }

        if (auto* cylCol = node->GetComponent<NFSEngine::CylinderCollider3DComponent>()) {
            cylCol->IsTrigger = true;
            cylCol->OnTriggerEnter = [parentSliceName](NFSEngine::GameObject* otherObj) {
                if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                    std::cout << "GRACZ WSZED£ NA PLASTER: " << parentSliceName << std::endl;
                }
                };
        }

        int childCount = node->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; ++i) {
            HookTriggersRecursive(node->GetTransform()->GetChild(i)->GetOwner(), parentSliceName);
        }
    }
};