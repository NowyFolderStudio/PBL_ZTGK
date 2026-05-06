#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace NFSEngine {
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
        bool IsColliding = false;
        glm::vec3 ContactNormal = glm::vec3(0.0f);
        float PenetrationDepth = 0.0f;
        std::vector<glm::vec3> ContactPoints;
    };
} // namespace NFSEngine