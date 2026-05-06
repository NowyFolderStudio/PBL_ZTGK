#include "Core/Physics/CollisionDetector.hpp"
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <cmath>

namespace NFSEngine {

    CollisionInfo CollisionDetector::CheckAABB(const AABB& a, const AABB& b) {
        CollisionInfo info;

        if ((a.Min.x <= b.Max.x && a.Max.x >= b.Min.x) && (a.Min.y <= b.Max.y && a.Max.y >= b.Min.y)
            && (a.Min.z <= b.Max.z && a.Max.z >= b.Min.z)) {

            info.IsColliding = true;

            float pX = std::min(a.Max.x - b.Min.x, b.Max.x - a.Min.x);
            float pY = std::min(a.Max.y - b.Min.y, b.Max.y - a.Min.y);
            float pZ = std::min(a.Max.z - b.Min.z, b.Max.z - a.Min.z);

            if (pX < pY && pX < pZ) {
                info.PenetrationDepth = pX;
                info.ContactNormal = (a.Min.x < b.Min.x) ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
            } else if (pY < pX && pY < pZ) {
                info.PenetrationDepth = pY;
                info.ContactNormal = (a.Min.y < b.Min.y) ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
            } else {
                info.PenetrationDepth = pZ;
                info.ContactNormal = (a.Min.z < b.Min.z) ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
            }
        }
        return info;
    }

    CollisionInfo CollisionDetector::CheckSphere(const Sphere& a, const Sphere& b) {
        CollisionInfo info;
        glm::vec3 diff = a.Center - b.Center;

        float distance = glm::length(diff);
        float radiusSum = a.Radius + b.Radius;

        if (distance < radiusSum) {
            info.IsColliding = true;
            info.PenetrationDepth = radiusSum - distance;

            if (distance > 0.0001f) {
                info.ContactNormal = diff / distance;
            } else {
                info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckAABBSphere(const AABB& box, const Sphere& sphere) {
        CollisionInfo info;

        glm::vec3 closestPoint = PhysicsMath::ClampPointToAABB(sphere.Center, box);

        glm::vec3 diff = sphere.Center - closestPoint;
        float distance = glm::length(diff);

        if (distance == 0.0f) {
            info.IsColliding = true;
            info.PenetrationDepth = sphere.Radius;
            info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            return info;
        }

        if (distance < sphere.Radius) {
            info.IsColliding = true;
            info.PenetrationDepth = sphere.Radius - distance;
            info.ContactNormal = diff / distance;
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCapsule(const Capsule& a, const Capsule& b) {
        CollisionInfo info;

        glm::vec3 bCenter = (b.PointA + b.PointB) * 0.5f;
        glm::vec3 closestOnA = PhysicsMath::ClosestPointOnSegment(a.PointA, a.PointB, bCenter);
        glm::vec3 closestOnB = PhysicsMath::ClosestPointOnSegment(b.PointA, b.PointB, closestOnA);

        closestOnA = PhysicsMath::ClosestPointOnSegment(a.PointA, a.PointB, closestOnB);

        glm::vec3 diff = closestOnA - closestOnB;
        float distance = glm::length(diff);
        float radiusSum = a.Radius + b.Radius;

        if (distance < radiusSum) {
            info.IsColliding = true;
            info.PenetrationDepth = radiusSum - distance;

            if (distance > 0.0001f) {
                info.ContactNormal = diff / distance;
            } else {
                info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCapsuleAABB(const Capsule& capsule, const AABB& box) {
        CollisionInfo info;

        glm::vec3 aabbCenter = (box.Min + box.Max) * 0.5f;

        glm::vec3 closestOnSegment = PhysicsMath::ClosestPointOnSegment(capsule.PointA, capsule.PointB, aabbCenter);
        glm::vec3 closestOnAABB = PhysicsMath::ClampPointToAABB(closestOnSegment, box);
        glm::vec3 finalClosestPoint = PhysicsMath::ClosestPointOnSegment(capsule.PointA, capsule.PointB, closestOnAABB);
        glm::vec3 diff = finalClosestPoint - closestOnAABB;
        float distance = glm::length(diff);

        if (distance < capsule.Radius) {
            info.IsColliding = true;

            info.PenetrationDepth = capsule.Radius - distance;

            if (distance > 0.0001f) {
                info.ContactNormal = diff / distance;
            } else {
                info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCapsuleOBB(const Capsule& capsule, const OBB& obb) {
        glm::quat inverseRotation = glm::inverse(obb.Rotation);

        glm::vec3 localPointA = inverseRotation * (capsule.PointA - obb.Center);
        glm::vec3 localPointB = inverseRotation * (capsule.PointB - obb.Center);

        Capsule localCapsule { localPointA, localPointB, capsule.Radius };
        AABB localAABB { -obb.HalfSize, obb.HalfSize };

        CollisionInfo info = CollisionDetector::CheckCapsuleAABB(localCapsule, localAABB);

        if (info.IsColliding) {
            info.ContactNormal = obb.Rotation * info.ContactNormal;
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCapsuleCylinder(const Capsule& capsule, const Cylinder& cylinder) {
        CollisionInfo info;
        glm::vec3 cylCenter = (cylinder.PointA + cylinder.PointB) * 0.5f;

        glm::vec3 closestOnSegment = PhysicsMath::ClosestPointOnSegment(capsule.PointA, capsule.PointB, cylCenter);
        glm::vec3 closestOnCylinder = PhysicsMath::ClampPointToCylinder(closestOnSegment, cylinder);
        glm::vec3 finalClosestPoint = PhysicsMath::ClosestPointOnSegment(capsule.PointA, capsule.PointB, closestOnCylinder);

        glm::vec3 diff = finalClosestPoint - closestOnCylinder;
        float distance = glm::length(diff);

        if (distance < capsule.Radius) {
            info.IsColliding = true;
            info.PenetrationDepth = capsule.Radius - distance;

            if (distance > 0.0001f) {
                info.ContactNormal = diff / distance;
            } else {
                info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere) {
        CollisionInfo info;

        glm::vec3 closestOnSegment = PhysicsMath::ClosestPointOnSegment(capsule.PointA, capsule.PointB, sphere.Center);

        glm::vec3 diff = closestOnSegment - sphere.Center;

        float distance = glm::length(diff);
        float radiusSum = capsule.Radius + sphere.Radius;

        if (distance < radiusSum) {
            info.IsColliding = true;

            info.PenetrationDepth = radiusSum - distance;

            if (distance > 0.0001f) {
                info.ContactNormal = diff / distance;
            } else {
                info.ContactNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }

    CollisionInfo CollisionDetector::CheckCylinder(const Cylinder& a, const Cylinder& b) {
        CollisionInfo info;
        float aMinY = std::min(a.PointA.y, a.PointB.y);
        float aMaxY = std::max(a.PointA.y, a.PointB.y);
        float bMinY = std::min(b.PointA.y, b.PointB.y);
        float bMaxY = std::max(b.PointA.y, b.PointB.y);

        if (aMaxY <= bMinY || aMinY >= bMaxY) {
            return info;
        }

        glm::vec2 aPos2D = glm::vec2(a.PointA.x, a.PointA.z);
        glm::vec2 bPos2D = glm::vec2(b.PointA.x, b.PointA.z);
        glm::vec2 diff2D = aPos2D - bPos2D;

        float distance = glm::length(diff2D);
        float radiusSum = a.Radius + b.Radius;

        if (distance < radiusSum) {
            info.IsColliding = true;

            float penetrationXZ = radiusSum - distance;

            float penY1 = aMaxY - bMinY;
            float penY2 = bMaxY - aMinY;
            float penetrationY = std::min(penY1, penY2);

            if (penetrationXZ < penetrationY) {
                info.PenetrationDepth = penetrationXZ;
                if (distance > 0.0001f) {
                    glm::vec2 normal2D = diff2D / distance;
                    info.ContactNormal = glm::vec3(normal2D.x, 0.0f, normal2D.y);
                } else {
                    info.ContactNormal = glm::vec3(1.0f, 0.0f, 0.0f);
                }
            } else {
                info.PenetrationDepth = penetrationY;
                info.ContactNormal = (penY1 < penY2) ? glm::vec3(0.0f, -1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        return info;
    }
} // namespace NFSEngine