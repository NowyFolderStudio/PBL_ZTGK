#pragma once

#include "Core/KeyCodes.hpp"
#include <NFSEngine.h>

class CharacterController : public NFSEngine::Component {
public:
    CharacterController(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { };

    std::string GetName() const override { return "CharacterController"; };

    void SetCameraTransform(NFSEngine::Transform* cameraTransform) { m_CameraTransform = cameraTransform; }

protected:
    virtual void OnAwake() override {
        p_RigidBody = GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!p_RigidBody) {
            NFS_CORE_ERROR("CharacterController component requires RigidBody3DComponent on the same GameObject!");
        }
    }
    virtual void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) {
        if (!p_RigidBody) return;
        float speed = 5.0f;

        float inputX = 0.0f;
        float inputZ = 0.0f;

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::W)) inputZ += 1.0f;
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::S)) inputZ -= 1.0f;
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::A)) inputX -= 1.0f;
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::D)) inputX += 1.0f;

        glm::vec3 moveDirection(0.0f);

        if ((inputX != 0.0f || inputZ != 0.0f) && m_CameraTransform) {
            glm::vec3 camPos = m_CameraTransform->GetPosition();
            glm::vec3 playerPos = GetOwner()->GetTransform()->GetPosition();

            glm::vec3 forward = playerPos - camPos;
            forward.y = 0.0f;
            forward = glm::normalize(forward);

            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

            moveDirection = (forward * inputZ) + (right * inputX);

            if (glm::length(moveDirection) > 0.001f) {
                moveDirection = glm::normalize(moveDirection);
            }

            float targetAngle = glm::degrees(atan2(moveDirection.x, moveDirection.z));

            GetOwner()->GetTransform()->SetRotation(glm::vec3(0.0f, targetAngle, 0.0f));
        }

        p_RigidBody->Velocity.x = moveDirection.x * speed;
        p_RigidBody->Velocity.z = moveDirection.z * speed;

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::Space)) {
            p_RigidBody->Velocity.y = 5.0f; // Jump velocity
        }
    }

private:
    NFSEngine::RigidBody3DComponent* p_RigidBody = nullptr;
    NFSEngine::Transform* m_CameraTransform = nullptr;
};