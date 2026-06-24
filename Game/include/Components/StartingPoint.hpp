#pragma once
#include "Components/CharacterController.hpp"
#include "Core/GameObject.hpp"
#include "Core/Tags.hpp"
#include <NFSEngine.h>
#include <vector>

class StartingPoint : public NFSEngine::Component {
public:
    StartingPoint(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "StartingPoint"; }

    void OnStart() override {
        auto players = GetOwner()->GetScene()->FindGameObjectsWithTag(NFSEngine::Tags::Player);
        for (auto* p : players) {
            if (p->GetTransform()->GetParent() == nullptr) { }
            p->GetComponent<CharacterController>()->SpawnPosition = GetOwner()->GetTransform()->GetWorldPosition();
            p->GetTransform()->SetWorldPosition(GetOwner()->GetTransform()->GetWorldPosition());
            p->GetTransform()->SetWorldRotation(GetOwner()->GetTransform()->GetWorldRotation());
            return;
            ;
        }
    }
};