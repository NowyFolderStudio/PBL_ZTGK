#pragma once

#include <glm/glm.hpp>

#include "Components/Component.hpp"

namespace NFSEngine
{
struct RigidBody3DComponent : public Component
{
    RigidBody3DComponent(GameObject* owner)
        : Component(owner)
    {
    }

    std::string GetName() const override { return "RigidBody3D"; }

    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 Acceleration = glm::vec3(0.0f);
    glm::vec3 PreviousPosition = glm::vec3(0.0f);

    float Mass = 1.0f;
    bool UseGravity = true;
};

struct BoxCollider3DComponent : public Component
{
    BoxCollider3DComponent(GameObject* owner)
        : Component(owner)
    {
    }

    std::string GetName() const override { return "BoxCollider3D"; }

    glm::vec3 Size = glm::vec3(1.0f);
    glm::vec3 Offset = glm::vec3(0.0f);

    bool IsTrigger = false;
};
}