#include "Core/Physics/PhysicsMath.hpp"
#include <algorithm>
#include <cmath>

namespace NFSEngine {

    glm::vec3 PhysicsMath::ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& point) {
        glm::vec3 ab = b - a;
        float lengthSquared = glm::dot(ab, ab);

        if (lengthSquared == 0.0f) return a;
        float t = glm::dot(point - a, ab) / lengthSquared;

        return a + glm::clamp(t, 0.0f, 1.0f) * ab;
    }

    glm::vec3 PhysicsMath::ClampPointToAABB(const glm::vec3& point, const AABB& box) {
        return glm::vec3(std::max(box.Min.x, std::min(point.x, box.Max.x)), std::max(box.Min.y, std::min(point.y, box.Max.y)),
                         std::max(box.Min.z, std::min(point.z, box.Max.z)));
    }

    glm::vec3 PhysicsMath::ClampPointToCylinder(const glm::vec3& point, const Cylinder& cyl) {
        glm::vec3 result = point;

        float minY = std::min(cyl.PointA.y, cyl.PointB.y);
        float maxY = std::max(cyl.PointA.y, cyl.PointB.y);
        result.y = std::clamp(point.y, minY, maxY);

        glm::vec2 p2D(point.x, point.z);
        glm::vec2 c2D(cyl.PointA.x, cyl.PointA.z);

        glm::vec2 diff2D = p2D - c2D;
        float distSq2D = glm::dot(diff2D, diff2D);

        if (distSq2D > cyl.Radius * cyl.Radius) {
            float dist2D = std::sqrt(distSq2D);
            glm::vec2 clamped2D = c2D + (diff2D / dist2D) * cyl.Radius;

            result.x = clamped2D.x;
            result.z = clamped2D.y;
        }

        return result;
    }

    float PhysicsMath::SqDistSegToSeg(const glm::vec3& p1, const glm::vec3& q1, const glm::vec3& p2, const glm::vec3& q2) {
        glm::vec3 d1 = q1 - p1;
        glm::vec3 d2 = q2 - p2;
        glm::vec3 r = p1 - p2;

        float a = glm::dot(d1, d1);
        float e = glm::dot(d2, d2);
        float f = glm::dot(d2, r);

        float s, t;
        float c = glm::dot(d1, r);
        float b = glm::dot(d1, d2);
        float denom = a * e - b * b;

        if (denom != 0.0f) {
            s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
        } else {
            s = 0.0f;
        }

        t = (b * s + f) / e;

        if (t < 0.0f) {
            t = 0.0f;
            s = glm::clamp(-c / a, 0.0f, 1.0f);
        } else if (t > 1.0f) {
            t = 1.0f;
            s = glm::clamp((b - c) / a, 0.0f, 1.0f);
        }

        glm::vec3 c1 = p1 + d1 * s;
        glm::vec3 c2 = p2 + d2 * t;
        glm::vec3 distVec = c1 - c2;

        return glm::dot(distVec, distVec);
    }
} // namespace NFSEngine