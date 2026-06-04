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

    struct Ray {
        glm::vec3 Origin;
        glm::vec3 Direction;
    };

    struct RaycastResult {
        bool Hit = false;
        float Distance = 0.0f;
        glm::vec3 Point;
        glm::vec3 Normal;
    };

    struct CollisionInfo {
        bool IsColliding = false;
        glm::vec3 ContactNormal = glm::vec3(0.0f);
        float PenetrationDepth = 0.0f;
        std::vector<glm::vec3> ContactPoints;
    };
} // namespace NFSEngine