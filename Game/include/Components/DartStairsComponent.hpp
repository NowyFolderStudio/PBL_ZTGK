#pragma once

#include <NFSEngine.h>
#include <vector>
#include <string>
#include <iostream>

class DartStairsComponent : public NFSEngine::Component {
public:
    DartStairsComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartStairsComponent"; }

    std::vector<NFSEngine::GameObject*> AllSlices;

    void StartTransition() {
        if (AllSlices.empty()) return;

        NFS_CORE_INFO("DartStairsComponent: Rozpoczynam podpinanie triggerow w plastrach...");

        for (auto* slice : AllSlices) {
            SearchAndHookCollidersRecursive(slice, slice->name);
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
    }

private:
    void SearchAndHookCollidersRecursive(NFSEngine::GameObject* node, const std::string& parentSliceName) {
        std::string nodeName = node->name;

        std::string lowerName = nodeName;
        for (auto& c : lowerName) c = std::tolower(c);

        if (lowerName.find("collider") != std::string::npos && lowerName.find("full") == std::string::npos) {
            auto* triggerObj = node->GetScene()->CreateGameObject("SensorTrigger_" + parentSliceName);
            triggerObj->GetTransform()->SetParent(node->GetTransform(), false);
            triggerObj->GetTransform()->SetPosition(node->GetTransform()->GetPosition());

            if (auto* originalBoxCol = node->GetComponent<NFSEngine::BoxCollider3DComponent>()) {
                auto& newTriggerCol = triggerObj->AddComponent<NFSEngine::BoxCollider3DComponent>();

                newTriggerCol.Size = originalBoxCol->Size + glm::vec3(0.5f, 0.0f, 0.0f);
                newTriggerCol.Offset = originalBoxCol->Offset + glm::vec3(0.0f, 0.25f, 0.0f);
                newTriggerCol.IsTrigger = true;

                newTriggerCol.OnTriggerEnter = [parentSliceName](NFSEngine::GameObject* otherObj) {
                    if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                        std::cout << "--> GRACZ STOI NA PLASTRO: " << parentSliceName << " <--" << std::endl;
                    }
                    };
            }
            else if (auto* originalCylCol = node->GetComponent<NFSEngine::CylinderCollider3DComponent>()) {
                auto& newTriggerCol = triggerObj->AddComponent<NFSEngine::CylinderCollider3DComponent>();

                newTriggerCol.Radius = originalCylCol->Radius;
                newTriggerCol.Height = originalCylCol->Height + 0.5f;
                newTriggerCol.Offset = originalCylCol->Offset + glm::vec3(0.0f, 0.25f, 0.0f);
                newTriggerCol.IsTrigger = true;

                newTriggerCol.OnTriggerEnter = [parentSliceName](NFSEngine::GameObject* otherObj) {
                    if (otherObj->CompareTag(NFSEngine::Tags::Player)) {
                        std::cout << "--> GRACZ STOI NA PLASTRO: " << parentSliceName << " <--" << std::endl;
                    }
                    };
            }
        }

        int childCount = node->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; ++i) {
            SearchAndHookCollidersRecursive(node->GetTransform()->GetChild(i)->GetOwner(), parentSliceName);
        }
    }
};