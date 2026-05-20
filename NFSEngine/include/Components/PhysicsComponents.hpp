#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine {

    enum class ColliderType {
        Box,
        Sphere,
        Capsule,
        Cylinder
    };

    struct ColliderComponent : public Component {
        ColliderType Type;
        bool IsTrigger = false;

        std::function<void(GameObject* other)> OnTriggerEnter = nullptr;
        std::function<void(GameObject* other)> OnTriggerStay = nullptr;
        std::function<void(GameObject* other)> OnTriggerExit = nullptr;

        std::function<void(GameObject* other, glm::vec3 contactNormal)> OnCollisionEnter = nullptr;

        ColliderComponent(GameObject* owner, ColliderType type)
            : Component(owner)
            , Type(type) { }
    };

    struct RigidBody3DComponent : public Component {
        RigidBody3DComponent(GameObject* owner)
            : Component(owner) { }

        std::string GetName() const override { return "RigidBody3D"; }

        glm::vec3 Velocity = glm::vec3(0.0f);
        glm::vec3 Acceleration = glm::vec3(0.0f);
        glm::vec3 PreviousPosition = glm::vec3(0.0f);

        float Mass = 1.0f;
        bool UseGravity = true;
        bool IsGrounded = false;

        bool IsTouchingWall = false;
        glm::vec3 WallNormal = glm::vec3(0.0f);
        glm::vec3 FloorNormal = glm::vec3(0.0f);

        GameObject* TouchedWallObject = nullptr;
        GameObject* TouchedFloorObject = nullptr;
    };

    struct BoxCollider3DComponent : public ColliderComponent {
        BoxCollider3DComponent(GameObject* owner)
            : ColliderComponent(owner, ColliderType::Box) { }

        std::string GetName() const override { return "BoxCollider3D"; }

        glm::vec3 Size = glm::vec3(1.0f);
        glm::vec3 Offset = glm::vec3(0.0f);

        void OnRender() override {
            Transform* t = m_Owner->GetComponent<Transform>();
            if (!t) return;

            glm::mat4 positionMat = glm::translate(glm::mat4(1.0f), t->GetPosition());
            glm::mat4 offsetMat = glm::translate(glm::mat4(1.0f), Offset);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), Size);
            glm::mat4 rotation = glm::mat4_cast(t->GetRotation());

            glm::mat4 debugTransform = positionMat * offsetMat * rotation * scaleMat;

            glm::vec4 color = IsTrigger ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

            Renderer::SubmitDebugBox(debugTransform, color);
        }
    };

    struct SphereCollider3DComponent : public ColliderComponent {
        SphereCollider3DComponent(GameObject* owner)
            : ColliderComponent(owner, ColliderType::Sphere) { }
        std::string GetName() const override { return "SphereCollider3D"; }

        float Radius = 1.0f;
        glm::vec3 Offset = glm::vec3(0.0f);
    };

    struct CapsuleCollider3DComponent : public ColliderComponent {
        CapsuleCollider3DComponent(GameObject* owner)
            : ColliderComponent(owner, ColliderType::Capsule) { }
        std::string GetName() const override { return "CapsuleCollider3D"; }

        float Radius = 1.0f;
        float Height = 1.0f;

        glm::vec3 Offset = glm::vec3(0.0f);
    };

    struct CylinderCollider3DComponent : public ColliderComponent {
        CylinderCollider3DComponent(GameObject* owner)
            : ColliderComponent(owner, ColliderType::Cylinder) { }
        std::string GetName() const override { return "CylinderCollider3D"; }

        float Radius = 1.0f;
        float Height = 2.0f;

        glm::vec3 Offset = glm::vec3(0.0f);
    };
} // namespace NFSEngine