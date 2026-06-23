#pragma once
#include <NFSEngine.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

#include "Components/Transform.hpp"

class RotatingObject : public NFSEngine::Component {
public:
    RotatingObject(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "RotatingObject"; }

    void SetRotation(glm::vec3 rotationDirection, float rotationSpeed) {
        m_RotationDirection = rotationDirection;
        m_RotationSpeed = rotationSpeed;
    }

protected:
    void OnAwake() override { m_Transform = GetOwner()->GetTransform(); }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        m_Transform->RotateAroundAxis(m_RotationDirection, m_RotationSpeed * static_cast<float>(deltaTime));
    }

private:
    NFSEngine::Transform* m_Transform;
    glm::vec3 m_RotationDirection;
    float m_RotationSpeed;
};
