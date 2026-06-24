#pragma once
#include <NFSEngine.h>
#include <functional>
#include "Renderer/Material.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/Tags.hpp"

class PortalComponent : public NFSEngine::Component {
public:
    std::string TargetName;
    std::function<void()> OnTriggerCallback;
    bool IsActive = true;

    std::shared_ptr<NFSEngine::Material> PortalMaterial = nullptr;

    PortalComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "PortalComponent"; }

    void Activate() {
        if (IsActive && OnTriggerCallback) {
            OnTriggerCallback();
            IsActive = false;
        }
    }

protected:
    float m_TimeAccumulator = 0.0f;

    void OnStart() override {
        if (auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>()) {
            collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (other && other->CompareTag(NFSEngine::Tags::Player)) {
                    Activate();
                }
            };
        }
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (PortalMaterial && IsActive) {
            m_TimeAccumulator += deltaTime.GetSeconds();
            PortalMaterial->SetFloat("u_Time", m_TimeAccumulator);
        }
    }

    void OnAwake() override { }
    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }
    void OnEnable() override { }
    void OnDisable() override { }
};