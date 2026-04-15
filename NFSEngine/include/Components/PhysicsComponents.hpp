#pragma once

#include <glm/glm.hpp>

#include "Components/Component.hpp"

namespace NFSEngine {

    enum class ColliderType {
        Box,
        Sphere,
        Capsule
    };

    struct ColliderComponent : public Component {
        ColliderType Type;
        bool IsTrigger = false;

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
    };

    struct BoxCollider3DComponent : public ColliderComponent {
        BoxCollider3DComponent(GameObject* owner)
            : ColliderComponent(owner, ColliderType::Box) { }

        std::string GetName() const override { return "BoxCollider3D"; }

        glm::vec3 Size = glm::vec3(1.0f);
        glm::vec3 Offset = glm::vec3(0.0f);
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
        float Height = 2.0f;

        glm::vec3 Offset = glm::vec3(0.0f);
    };
} // namespace NFSEngine