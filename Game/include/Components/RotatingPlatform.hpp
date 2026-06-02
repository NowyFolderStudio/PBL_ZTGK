#pragma once

#include <NFSEngine.h>

class RotatingPlatform : public NFSEngine::Component {
public:
    RotatingPlatform(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "RotatingPlatform"; }

    glm::vec3 RotationSpeed = glm::vec3(0.0f, 90.0f, 0.0f);

private:
    NFSEngine::RigidBody3DComponent* m_Rigidbody = nullptr;

protected:
    void OnAwake() override {
        m_Rigidbody = GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!m_Rigidbody) {
            m_Rigidbody = &GetOwner()->AddComponent<NFSEngine::RigidBody3DComponent>();
        }

        m_Rigidbody->IsKinematic = true;
        m_Rigidbody->UseGravity = false;
    }

    void OnStart() override { }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_Rigidbody) {
            m_Rigidbody->AngularVelocity = RotationSpeed;
        }
    }

    void OnEnable() override { }

    void OnDisable() override { }
};
