#pragma once

#include <NFSEngine.h>
#include "Components/ModelComponent.hpp"
#include "Components/PhysicsComponents.hpp" 
#include "Components/DartRainAttackManager.hpp" 
#include "Components/DartStairsComponent.hpp"
#include <string>
#include <vector>

class DartController : public NFSEngine::Component {
public:
    DartController(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartController"; }

    glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

    void SetDetailedCollidersActive(bool detailedActive) {
        if (m_FullCollider) {
            m_FullCollider->SetActive(!detailedActive);
        }

        for (auto* group : m_ColliderGroups) {
            if (group) {
                group->SetActive(detailedActive);
            }
        }
    }

    void SetRainAttackActive(bool isActive) {
        if (m_RainAttack) {
            m_RainAttack->IsActive = isActive;
        }
    }

    void StartStairsPhase() {
        SetDetailedCollidersActive(true); 

        if (m_StairsManager) {
            m_StairsManager->StartTransition();
        }
    }

protected:
    void OnStart() override {
        NFSEngine::GameObject* blackSlice = nullptr;
        NFSEngine::GameObject* whiteSlice = nullptr;

        int childCount = GetOwner()->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; i++) {
            auto* child = GetOwner()->GetTransform()->GetChild(i)->GetOwner();
            if (child->name.find("SliceBlack1") != std::string::npos) {
                blackSlice = child;
            }
            else if (child->name.find("SliceWhite1") != std::string::npos) {
                whiteSlice = child;
            }
            else if (child->name.find("FullCollider") != std::string::npos) {
                m_FullCollider = child;
            }
        }

        if (!blackSlice || !whiteSlice) {
            NFS_CORE_WARN("DartController: Nie znaleziono SliceBlack1 lub SliceWhite1!");
            return;
        }

        m_AllSlices.push_back(blackSlice);
        m_AllSlices.push_back(whiteSlice);

        for (int i = 1; i < 9; i++) {
            float currentAngle = i * 40.0f;
            auto* bClone = CloneSlice(blackSlice, "SliceBlack" + std::to_string(i + 1), currentAngle);
            auto* wClone = CloneSlice(whiteSlice, "SliceWhite" + std::to_string(i + 1), currentAngle);
            
            m_AllSlices.push_back(bClone);
            m_AllSlices.push_back(wClone);
        }

        CollectAndDeactivateColliders(GetOwner());
        
        SetDetailedCollidersActive(false);

        m_RainAttack = &GetOwner()->AddComponent<DartRainAttackComponent>();
        m_RainAttack->IsActive = false;
        
        m_RainAttack->TargetTrack = "Kick"; 
        m_RainAttack->NotesDelay = 1; 

        m_RainAttack->PlayerTransform = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);
        
        m_StairsManager = &GetOwner()->AddComponent<DartStairsComponent>();
        m_StairsManager->AllSlices = m_AllSlices; 

        StartStairsPhase();
    }

public:
    void OnEvent(NFSEngine::Event& e) {
        if (m_RainAttack) {
            m_RainAttack->OnEvent(e);
        }
    }

private:
    std::vector<NFSEngine::GameObject*> m_ColliderGroups;
    std::vector<NFSEngine::GameObject*> m_AllSlices;
    NFSEngine::GameObject* m_FullCollider = nullptr;

    DartRainAttackComponent* m_RainAttack = nullptr;
    DartStairsComponent* m_StairsManager = nullptr;

    void CollectAndDeactivateColliders(NFSEngine::GameObject* node) {
        if ((node->name.find("colliders") != std::string::npos || node->name.find("Colliders") != std::string::npos) &&
             node->name.find("FullCollider") == std::string::npos) {
            m_ColliderGroups.push_back(node);
        }

        int childCount = node->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; i++) {
            CollectAndDeactivateColliders(node->GetTransform()->GetChild(i)->GetOwner());
        }
    }

    NFSEngine::GameObject* CloneNode(NFSEngine::GameObject* original, NFSEngine::GameObject* newParent, const std::string& newName) {
        auto* copy = GetOwner()->GetScene()->CreateGameObject(newName);

        if (newParent) {
            copy->GetTransform()->SetParent(newParent->GetTransform(), false);
        }

        copy->GetTransform()->SetPosition(original->GetTransform()->GetPosition());
        copy->GetTransform()->SetRotation(original->GetTransform()->GetRotation());
        copy->GetTransform()->SetScale(original->GetTransform()->GetScale());

        if (auto* origModel = original->GetComponent<NFSEngine::ModelComponent>()) {
            auto shader = origModel->GetShader();
            auto mat = origModel->GetMaterial(0);

            auto& newModelComp = copy->AddComponent<NFSEngine::ModelComponent>(shader, mat);
            for (const auto& lod : origModel->GetLODs()) {
                newModelComp.AddLOD(lod.ModelData, lod.MaxDistance);
            }
        }

        if (auto* origBoxCol = original->GetComponent<NFSEngine::BoxCollider3DComponent>()) {
            auto& newBoxCol = copy->AddComponent<NFSEngine::BoxCollider3DComponent>();
            newBoxCol.Size = origBoxCol->Size;
            newBoxCol.Offset = origBoxCol->Offset;
            newBoxCol.IsTrigger = origBoxCol->IsTrigger;
        }
        
        if (auto* origCylCol = original->GetComponent<NFSEngine::CylinderCollider3DComponent>()) {
            auto& newCylCol = copy->AddComponent<NFSEngine::CylinderCollider3DComponent>();
            newCylCol.Radius = origCylCol->Radius;
            newCylCol.Height = origCylCol->Height;
            newCylCol.Offset = origCylCol->Offset;
            newCylCol.IsTrigger = origCylCol->IsTrigger;
        }

        if (auto* origSphereCol = original->GetComponent<NFSEngine::SphereCollider3DComponent>()) {
            auto& newSphereCol = copy->AddComponent<NFSEngine::SphereCollider3DComponent>();
            newSphereCol.Radius = origSphereCol->Radius;
            newSphereCol.Offset = origSphereCol->Offset;
            newSphereCol.IsTrigger = origSphereCol->IsTrigger;
        }
        
        if (auto* origCapCol = original->GetComponent<NFSEngine::CapsuleCollider3DComponent>()) {
            auto& newCapCol = copy->AddComponent<NFSEngine::CapsuleCollider3DComponent>();
            newCapCol.Radius = origCapCol->Radius;
            newCapCol.Height = origCapCol->Height;
            newCapCol.Offset = origCapCol->Offset;
            newCapCol.IsTrigger = origCapCol->IsTrigger;
        }

        int childCount = original->GetTransform()->GetChildCount();
        for (int i = 0; i < childCount; i++) {
            auto* origChild = original->GetTransform()->GetChild(i)->GetOwner();
            CloneNode(origChild, copy, origChild->name + "_copy");
        }

        return copy;
    }

    NFSEngine::GameObject* CloneSlice(NFSEngine::GameObject* original, const std::string& newName, float angleOffset) {
        
        NFSEngine::GameObject* parentObj = nullptr;
        if (original->GetTransform()->GetParent()) {
            parentObj = original->GetTransform()->GetParent()->GetOwner();
        }

        auto* copy = CloneNode(original, parentObj, newName);
        copy->GetTransform()->Rotate(RotationAxis * angleOffset);

        return copy;
    }
};