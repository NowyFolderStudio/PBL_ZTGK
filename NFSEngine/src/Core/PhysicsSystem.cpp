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

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        GameObject* objA = gameObjects[i].get();
        auto* rigidBody = objA->GetComponent<RigidBody3DComponent>();
        auto* colA = objA->GetComponent<BoxCollider3DComponent>();

        if (!rigidBody || !objA->IsActive()) continue;

        auto* transform = objA->GetTransform();

        if (rigidBody->UseGravity)
        {
            rigidBody->Acceleration += Gravity;
        }
        rigidBody->Velocity += rigidBody->Acceleration * dt;
        rigidBody->Acceleration = glm::vec3(0.0f);

        glm::vec3 moveDelta = rigidBody->Velocity * dt;

        transform->Move(glm::vec3(moveDelta.x, 0.0f, 0.0f));
        for (size_t j = 0; j < gameObjects.size(); j++)
        {
            if (i == j) continue;
            GameObject* objB = gameObjects[j].get();
            auto* colB = objB->GetComponent<BoxCollider3DComponent>();

            if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger)
            {
                transform->Move(glm::vec3(-moveDelta.x, 0.0f, 0.0f));
                rigidBody->Velocity.x = 0.0f;
                break;
            }
        }

        transform->Move(glm::vec3(0.0f, moveDelta.y, 0.0f));
        for (size_t j = 0; j < gameObjects.size(); j++)
        {
            if (i == j) continue;
            GameObject* objB = gameObjects[j].get();
            auto* colB = objB->GetComponent<BoxCollider3DComponent>();

            if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger)
            {
                transform->Move(glm::vec3(0.0f, -moveDelta.y, 0.0f));
                rigidBody->Velocity.y = 0.0f;
                break;
            }
        }

        transform->Move(glm::vec3(0.0f, 0.0f, moveDelta.z));
        for (size_t j = 0; j < gameObjects.size(); j++)
        {
            if (i == j) continue;
            GameObject* objB = gameObjects[j].get();
            auto* colB = objB->GetComponent<BoxCollider3DComponent>();

            if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger)
            {
                transform->Move(glm::vec3(0.0f, 0.0f, -moveDelta.z));
                rigidBody->Velocity.z = 0.0f;
                break;
            }
        }
    }
};
}