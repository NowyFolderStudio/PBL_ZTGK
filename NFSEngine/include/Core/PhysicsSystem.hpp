#pragma once

#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine
{
class PhysicsSystem
{
public:
    void Update(std::vector<std::unique_ptr<GameObject>>& gameObjects, DeltaTime deltaTime);
    glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

private:
    struct AABB
    {
        glm::vec3 Min;
        glm::vec3 Max;
    };

    struct Sphere
    {
        glm::vec3 Center;
        float Radius;
    };

    struct Capsule
    {
        glm::vec3 PointA;
        glm::vec3 PointB;
        float Radius;
    };

    static bool MathCheckAABB(const AABB& a, const AABB& b);
    static bool MathCheckSphere(const Sphere& a, const Sphere& b);
    static bool MathCheckCapsule(const Capsule& a, const Capsule& b);

    static bool MathCheckAABBSphere(const AABB& box, const Sphere& sphere);
    static bool MathCheckCapsuleAABB(const Capsule& capsule, const AABB& box);
    static bool MathCheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere);

    static PhysicsSystem::AABB GetAABB(Transform* transform, BoxCollider3DComponent* collider);
    static PhysicsSystem::Sphere GetSphere(Transform* transform, SphereCollider3DComponent* collider);
    static PhysicsSystem::Capsule GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider);

    static bool CheckCollision(GameObject* a, GameObject* b);
};
}