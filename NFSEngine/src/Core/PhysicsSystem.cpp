#include "Core/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine
{

bool PhysicsSystem::MathCheckAABB(const AABB& a, const AABB& b)
{
    return (a.Min.x <= b.Max.x && a.Max.x >= b.Min.x) && (a.Min.y <= b.Max.y && a.Max.y >= b.Min.y)
        && (a.Min.z <= b.Max.z && a.Max.z >= b.Min.z);
};

bool PhysicsSystem::CheckCollision(GameObject* a, GameObject* b)
{
    auto* colliderA = a->GetComponent<BoxCollider3DComponent>();
    auto* colliderB = b->GetComponent<BoxCollider3DComponent>();

    if (!colliderA || !colliderB) return false;

    AABB boxA;
    boxA.Min = a->GetTransform()->GetPosition() + colliderA->Offset - colliderA->Size * 0.5f;
    boxA.Max = a->GetTransform()->GetPosition() + colliderA->Offset + colliderA->Size * 0.5f;

    AABB boxB;
    boxB.Min = b->GetTransform()->GetPosition() + colliderB->Offset - colliderB->Size * 0.5f;
    boxB.Max = b->GetTransform()->GetPosition() + colliderB->Offset + colliderB->Size * 0.5f;
    return MathCheckAABB(boxA, boxB);
};

void PhysicsSystem::Update(std::vector<std::unique_ptr<GameObject>>& gameObjects, DeltaTime deltaTime)
{
    float dt = static_cast<float>(deltaTime);
    for (auto& gameObject : gameObjects)
    {
        if (auto* rigidBody = gameObject->GetComponent<RigidBody3DComponent>())
        {
            auto* transform = gameObject->GetTransform();
            rigidBody->PreviousPosition = transform->GetPosition();

            if (rigidBody->UseGravity)
            {
                rigidBody->Acceleration += Gravity;
            }

            rigidBody->Velocity += rigidBody->Acceleration * dt;

            transform->Move(rigidBody->Velocity * dt);

            rigidBody->Acceleration = glm::vec3(0.0f);
        }
    }

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        for (size_t j = i + 1; j < gameObjects.size(); j++)
        {
            GameObject* objA = gameObjects[i].get();
            GameObject* objB = gameObjects[j].get();

            if (!objA->IsActive() || !objB->IsActive()) continue;

            if (CheckCollision(objA, objB))
            {
                auto* rigidBodyA = objA->GetComponent<RigidBody3DComponent>();
                auto* colliderA = objA->GetComponent<BoxCollider3DComponent>();
                auto* colliderB = objB->GetComponent<BoxCollider3DComponent>();

                if (rigidBodyA && !colliderA->IsTrigger && !colliderB->IsTrigger)
                {
                    objA->GetTransform()->SetPosition(rigidBodyA->PreviousPosition);
                    rigidBodyA->Velocity = glm::vec3(0.0f);
                }

                auto* rigidBodyB = objB->GetComponent<RigidBody3DComponent>();

                if (rigidBodyB && !colliderA->IsTrigger && !colliderB->IsTrigger)
                {
                    objB->GetTransform()->SetPosition(rigidBodyB->PreviousPosition);
                    rigidBodyB->Velocity = glm::vec3(0.0f);
                }
            }
        }
    }
};
}