#include "Core/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine {

    bool PhysicsSystem::MathCheckAABB(const AABB& a, const AABB& b) {
        return (a.Min.x <= b.Max.x && a.Max.x >= b.Min.x) && (a.Min.y <= b.Max.y && a.Max.y >= b.Min.y)
            && (a.Min.z <= b.Max.z && a.Max.z >= b.Min.z);
    };

    bool PhysicsSystem::MathCheckSphere(const Sphere& a, const Sphere& b) {
        float distance = glm::distance(a.Center, b.Center);
        float radiusSum = a.Radius + b.Radius;

        return distance < radiusSum;
    }

    bool PhysicsSystem::MathCheckAABBSphere(const AABB& box, const Sphere& sphere) {
        float closestX = std::max(box.Min.x, std::min(sphere.Center.x, box.Max.x));
        float closestY = std::max(box.Min.y, std::min(sphere.Center.y, box.Max.y));
        float closestZ = std::max(box.Min.z, std::min(sphere.Center.z, box.Max.z));

        float distanceSquared = (closestX - sphere.Center.x) * (closestX - sphere.Center.x)
            + (closestY - sphere.Center.y) * (closestY - sphere.Center.y)
            + (closestZ - sphere.Center.z) * (closestZ - sphere.Center.z);

        return distanceSquared < (sphere.Radius * sphere.Radius);
    }

    bool PhysicsSystem::MathCheckCapsule(const Capsule& a, const Capsule& b) {
        // For simplicity, we will not implement capsule collision check in this example
        // Later we will add capsule collider, so we will need to implement this function
        return false;
    }

    bool PhysicsSystem::MathCheckCapsuleAABB(const Capsule& capsule, const AABB& box) {
        // For simplicity, we will not implement capsule-AABB collision check in this example
        // Later we will add capsule collider, so we will need to implement this function
        return false;
    }

    bool PhysicsSystem::MathCheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere) {
        // For simplicity, we will not implement capsule-sphere collision check in this example
        // Later we will add capsule collider, so we will need to implement this function
        return false;
    }

    bool PhysicsSystem::CheckCollision(GameObject* a, GameObject* b) {
        auto* colliderA = a->GetComponent<ColliderComponent>();
        auto* colliderB = b->GetComponent<ColliderComponent>();

        if (!colliderA || !colliderB) return false;

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Box) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);

            return MathCheckAABB(GetAABB(a->GetTransform(), boxA), GetAABB(b->GetTransform(), boxB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Sphere) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);

            return MathCheckSphere(GetSphere(a->GetTransform(), sphereA), GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Sphere) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);

            return MathCheckAABBSphere(GetAABB(a->GetTransform(), boxA), GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Box) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);

            return MathCheckAABBSphere(GetAABB(b->GetTransform(), boxB), GetSphere(a->GetTransform(), sphereA));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Capsule) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);

            return MathCheckCapsule(GetCapsule(a->GetTransform(), capsuleA), GetCapsule(b->GetTransform(), capsuleB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Capsule) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);

            return MathCheckCapsuleSphere(GetCapsule(b->GetTransform(), capsuleB), GetSphere(a->GetTransform(), sphereA));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Sphere) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);

            return MathCheckCapsuleSphere(GetCapsule(a->GetTransform(), capsuleA), GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Box) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);

            return MathCheckCapsuleAABB(GetCapsule(a->GetTransform(), capsuleA), GetAABB(b->GetTransform(), boxB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Capsule) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);

            return MathCheckCapsuleAABB(GetCapsule(b->GetTransform(), capsuleB), GetAABB(a->GetTransform(), boxA));
        }

        return false;
    };

    void PhysicsSystem::Update(std::vector<std::unique_ptr<GameObject>>& gameObjects, DeltaTime deltaTime) {
        float dt = static_cast<float>(deltaTime);

        for (size_t i = 0; i < gameObjects.size(); i++) {
            GameObject* objA = gameObjects[i].get();
            auto* rigidBody = objA->GetComponent<RigidBody3DComponent>();
            auto* colA = objA->GetComponent<ColliderComponent>();

            if (!rigidBody || !objA->IsActive()) continue;

            auto* transform = objA->GetTransform();

            if (rigidBody->UseGravity) {
                rigidBody->Acceleration += Gravity;
            }
            rigidBody->Velocity += rigidBody->Acceleration * dt;
            rigidBody->Acceleration = glm::vec3(0.0f);

            glm::vec3 moveDelta = rigidBody->Velocity * dt;

            transform->Move(glm::vec3(moveDelta.x, 0.0f, 0.0f));
            for (size_t j = 0; j < gameObjects.size(); j++) {
                if (i == j) continue;
                GameObject* objB = gameObjects[j].get();
                auto* colB = objB->GetComponent<ColliderComponent>();

                if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger) {
                    transform->Move(glm::vec3(-moveDelta.x, 0.0f, 0.0f));
                    rigidBody->Velocity.x = 0.0f;
                    break;
                }
            }

            transform->Move(glm::vec3(0.0f, moveDelta.y, 0.0f));
            for (size_t j = 0; j < gameObjects.size(); j++) {
                if (i == j) continue;
                GameObject* objB = gameObjects[j].get();
                auto* colB = objB->GetComponent<ColliderComponent>();

                if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger) {
                    transform->Move(glm::vec3(0.0f, -moveDelta.y, 0.0f));
                    rigidBody->Velocity.y = 0.0f;
                    break;
                }
            }

            transform->Move(glm::vec3(0.0f, 0.0f, moveDelta.z));
            for (size_t j = 0; j < gameObjects.size(); j++) {
                if (i == j) continue;
                GameObject* objB = gameObjects[j].get();
                auto* colB = objB->GetComponent<ColliderComponent>();

                if (objB->IsActive() && colB && CheckCollision(objA, objB) && !colA->IsTrigger && !colB->IsTrigger) {
                    transform->Move(glm::vec3(0.0f, 0.0f, -moveDelta.z));
                    rigidBody->Velocity.z = 0.0f;
                    break;
                }
            }
        }
    };

    PhysicsSystem::AABB PhysicsSystem::GetAABB(Transform* transform, BoxCollider3DComponent* collider) {
        PhysicsSystem::AABB box;
        box.Min = transform->GetPosition() + collider->Offset - collider->Size * 0.5f;
        box.Max = transform->GetPosition() + collider->Offset + collider->Size * 0.5f;

        return box;
    };

    PhysicsSystem::Sphere PhysicsSystem::GetSphere(Transform* transform, SphereCollider3DComponent* collider) {
        PhysicsSystem::Sphere sphere;
        sphere.Center = transform->GetPosition() + collider->Offset;
        sphere.Radius = collider->Radius;

        return sphere;
    };

    PhysicsSystem::Capsule PhysicsSystem::GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider) {
        PhysicsSystem::Capsule capsule;

        return capsule;
    };

} // namespace NFSEngine