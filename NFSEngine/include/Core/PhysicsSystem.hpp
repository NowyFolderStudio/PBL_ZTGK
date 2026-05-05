#pragma once

#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"
#include <memory>
#include <vector>

namespace NFSEngine {
    class PhysicsSystem {
    public:
        void Update(std::vector<std::unique_ptr<GameObject>>& gameObjects, DeltaTime deltaTime);
        glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        struct AABB {
            glm::vec3 Min;
            glm::vec3 Max;
        };

        struct OBB {
            glm::vec3 Center;
            glm::vec3 HalfSize;
            glm::quat Rotation;
        };

        struct Sphere {
            glm::vec3 Center;
            float Radius;
        };

        struct Capsule {
            glm::vec3 PointA;
            glm::vec3 PointB;
            float Radius;
        };

        struct Cylinder {
            glm::vec3 PointA;
            glm::vec3 PointB;
            float Radius;
        };

        struct CollisionInfo {
            bool IsColliding;
            glm::vec3 ContactNormal;
            float PenetrationDepth;
        };

        static glm::vec3 ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& point);
        static glm::vec3 ClampPointToAABB(const glm::vec3& point, const AABB& aabb);
        static glm::vec3 ClampPointToCylinder(const glm::vec3& point, const Cylinder& cylinder);
        static float SqDistSegToSeg(const glm::vec3& p1, const glm::vec3& q1, const glm::vec3& p2, const glm::vec3& q2);

        static bool MathCheckAABB(const AABB& a, const AABB& b);
        static bool MathCheckSphere(const Sphere& a, const Sphere& b);
        static bool MathCheckCapsule(const Capsule& a, const Capsule& b);
        static bool MathCheckCylinder(const Cylinder& a, const Cylinder& b);

        static bool MathCheckAABBSphere(const AABB& aabb, const Sphere& sphere);
        static bool MathCheckCapsuleAABB(const Capsule& capsule, const AABB& aabb);
        static bool MathCheckCapsuleOBB(const Capsule& capsule, const OBB& obb);
        static bool MathCheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere);
        static bool MathCheckCapsuleCylinder(const Capsule& capsule, const Cylinder& cylinder);

        static PhysicsSystem::AABB GetAABB(Transform* transform, BoxCollider3DComponent* collider);
        static PhysicsSystem::OBB GetOBB(Transform* transform, BoxCollider3DComponent* collider);
        static PhysicsSystem::Sphere GetSphere(Transform* transform, SphereCollider3DComponent* collider);
        static PhysicsSystem::Capsule GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider);
        static PhysicsSystem::Cylinder GetCylinder(Transform* transform, CylinderCollider3DComponent* collider);

        static bool CheckCollision(GameObject* a, GameObject* b);
    };
} // namespace NFSEngine