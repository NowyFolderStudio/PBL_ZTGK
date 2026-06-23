#pragma once
#include "Components/Component.hpp"
#include "Core/GameObject.hpp"

class ActivatebleComponent : public NFSEngine::Component {
public:
    ActivatebleComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "ActivatableObject"; }

    virtual void OnActivationEnter() { };
    virtual void OnActivationStay() { };
    virtual void OnActivationExit() { };
};