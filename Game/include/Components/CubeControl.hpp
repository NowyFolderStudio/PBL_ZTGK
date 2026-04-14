#pragma once

#include "Core/KeyCodes.hpp"
#include <NFSEngine.h>

class CubeControl : public NFSEngine::Component
{
public:
    CubeControl(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { };

    std::string GetName() const override { return "CubeControl"; };

protected:
    virtual void OnAwake() override
    {
        p_RigidBody = GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!p_RigidBody)
        {
            NFS_CORE_ERROR("CubeControl component requires RigidBody3DComponent on the same GameObject!");
        }
    }
    virtual void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) {
        if (!p_RigidBody)
            return;
        float speed = 5.0f;

        glm::vec3 currentVelocity = p_RigidBody->Velocity;
        glm::vec3 inputMovement(0.0f);

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::A)) inputMovement.x -= speed;
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::D)) inputMovement.x += speed;

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::W)) inputMovement.z -= speed;
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::S)) inputMovement.z += speed;

        p_RigidBody->Velocity.x = inputMovement.x;
        p_RigidBody->Velocity.z = inputMovement.z;

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::Space))
        {
            p_RigidBody->Velocity.y = 5.0f;
        }
    }

private:
    NFSEngine::RigidBody3DComponent* p_RigidBody = nullptr;
};