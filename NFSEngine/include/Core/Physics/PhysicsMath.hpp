#pragma once
#include "PhysicsPrimitives.hpp"

namespace NFSEngine {
    class PhysicsMath {
    public:
        static glm::vec3 ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& point);
        static glm::vec3 ClampPointToAABB(const glm::vec3& point, const AABB& aabb);
        static glm::vec3 ClampPointToCylinder(const glm::vec3& point, const Cylinder& cylinder);
        static float SqDistSegToSeg(const glm::vec3& p1, const glm::vec3& q1, const glm::vec3& p2, const glm::vec3& q2);
    };
} // namespace NFSEngine