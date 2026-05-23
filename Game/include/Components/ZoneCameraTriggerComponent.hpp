#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Components/CameraController.hpp"

class ZoneCameraTriggerComponent : public NFSEngine::Component {
public:
    explicit ZoneCameraTriggerComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "ZoneCameraTriggerComponent"; }

    float CustomYaw = 0.0f;
    float CustomPitch = 20.0f;
    float CustomDistance = 15.0f;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            ApplyToCamera(true);
        };

        collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;
            ApplyToCamera(false);
        };
    }

private:
    void ApplyToCamera(bool active) {
        auto* scene = m_Owner->GetScene();
        if (!scene) return;
        for (const auto& go : scene->GetAllGameObjects()) {
            auto* camCtrl = go->GetComponent<NFSEngine::CameraController>();
            if (camCtrl) {
                if (active)
                    camCtrl->SetCameraOverride(CustomYaw, CustomPitch, CustomDistance);
                else
                    camCtrl->ClearCameraOverride();
                break;
            }
        }
    }
};
