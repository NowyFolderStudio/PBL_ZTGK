#pragma once
#include "PhysicsPrimitives.hpp"
#include "PhysicsMath.hpp"

namespace NFSEngine {
    class CollisionDetector {
    public:
        static CollisionInfo CheckAABB(const AABB& a, const AABB& b);
        static CollisionInfo CheckSphere(const Sphere& a, const Sphere& b);
        static CollisionInfo CheckCapsule(const Capsule& a, const Capsule& b);
        static CollisionInfo CheckCylinder(const Cylinder& a, const Cylinder& b);

        static CollisionInfo CheckAABBSphere(const AABB& aabb, const Sphere& sphere);
        static CollisionInfo CheckCapsuleAABB(const Capsule& capsule, const AABB& aabb);
        static CollisionInfo CheckCapsuleOBB(const Capsule& capsule, const OBB& obb);
        static CollisionInfo CheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere);
        static CollisionInfo CheckCapsuleCylinder(const Capsule& capsule, const Cylinder& cylinder);

        static bool CheckRayAABB(const Ray& ray, const AABB& aabb, float& outDistance);
        static bool CheckRayOBB(const Ray& ray, const OBB& obb, float& outDistance);
        static bool CheckRaySphere(const Ray& ray, const Sphere& sphere, float& outDistance);
        static bool CheckRayCapsule(const Ray& ray, const Capsule& capsule, float& outDistance);
    };
} // namespace NFSEngine