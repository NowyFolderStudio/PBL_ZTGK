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

        if (distance > 0.0001f) {
            if (distance < sphere.Radius) {
                info.IsColliding = true;
                info.PenetrationDepth = sphere.Radius - distance;
                info.ContactNormal = diff / distance;
            }
        } else {
            info.IsColliding = true;

            float pX = std::min(sphere.Center.x - box.Min.x, box.Max.x - sphere.Center.x);
            float pY = std::min(sphere.Center.y - box.Min.y, box.Max.y - sphere.Center.y);
            float pZ = std::min(sphere.Center.z - box.Min.z, box.Max.z - sphere.Center.z);

            if (pX < pY && pX < pZ) {
                info.PenetrationDepth = pX + sphere.Radius;
                info.ContactNormal
                    = (sphere.Center.x - box.Min.x < box.Max.x - sphere.Center.x) ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
            } else if (pY < pX && pY < pZ) {
                info.PenetrationDepth = pY + sphere.Radius;
                info.ContactNormal
                    = (sphere.Center.y - box.Min.y < box.Max.y - sphere.Center.y) ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
            } else {
                info.PenetrationDepth = pZ + sphere.Radius;
                info.ContactNormal
                    = (sphere.Center.z - box.Min.z < box.Max.z - sphere.Center.z) ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
            }
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

        if (distance > 0.0001f) {
            if (distance < capsule.Radius) {
                info.IsColliding = true;
                info.PenetrationDepth = capsule.Radius - distance;
                info.ContactNormal = diff / distance;
            }
        } else {
            info.IsColliding = true;

            float pX = std::min(closestOnSegment.x - box.Min.x, box.Max.x - closestOnSegment.x);
            float pY = std::min(closestOnSegment.y - box.Min.y, box.Max.y - closestOnSegment.y);
            float pZ = std::min(closestOnSegment.z - box.Min.z, box.Max.z - closestOnSegment.z);

            if (pX < pY && pX < pZ) {
                info.PenetrationDepth = pX + capsule.Radius;
                info.ContactNormal = (closestOnSegment.x - box.Min.x < box.Max.x - closestOnSegment.x) ? glm::vec3(-1, 0, 0)
                                                                                                       : glm::vec3(1, 0, 0);
            } else if (pY < pX && pY < pZ) {
                info.PenetrationDepth = pY + capsule.Radius;
                info.ContactNormal = (closestOnSegment.y - box.Min.y < box.Max.y - closestOnSegment.y) ? glm::vec3(0, -1, 0)
                                                                                                       : glm::vec3(0, 1, 0);
            } else {
                info.PenetrationDepth = pZ + capsule.Radius;
                info.ContactNormal = (closestOnSegment.z - box.Min.z < box.Max.z - closestOnSegment.z) ? glm::vec3(0, 0, -1)
                                                                                                       : glm::vec3(0, 0, 1);
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

    bool CollisionDetector::CheckRayAABB(const Ray& ray, const AABB& aabb, float& outDistance) {
        glm::vec3 invDir = 1.0f / ray.Direction;
        glm::vec3 t1 = (aabb.Min - ray.Origin) * invDir;
        glm::vec3 t2 = (aabb.Max - ray.Origin) * invDir;
        glm::vec3 tMin = glm::min(t1, t2);
        glm::vec3 tMax = glm::max(t1, t2);
        float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
        if (tNear > tFar || tFar < 0.0f) return false;
        outDistance = glm::max(0.0f, tNear);
        return true;
    }

    bool CollisionDetector::CheckRayOBB(const Ray& ray, const OBB& obb, float& outDistance) {
        glm::mat3 rot = glm::mat3_cast(obb.Rotation);
        glm::vec3 localOrigin = glm::transpose(rot) * (ray.Origin - obb.Center);
        glm::vec3 localDir = glm::transpose(rot) * ray.Direction;
        AABB localAABB{ -obb.HalfSize, obb.HalfSize };
        Ray localRay{ localOrigin, localDir };
        return CheckRayAABB(localRay, localAABB, outDistance);
    }

    bool CollisionDetector::CheckRaySphere(const Ray& ray, const Sphere& sphere, float& outDistance) {
        glm::vec3 oc = ray.Origin - sphere.Center;
        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(oc, ray.Direction);
        float c = glm::dot(oc, oc) - sphere.Radius * sphere.Radius;
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return false;
        float sqrtD = glm::sqrt(discriminant);
        float t = (-b - sqrtD) / (2.0f * a);
        if (t < 0.0f) t = (-b + sqrtD) / (2.0f * a);
        if (t < 0.0f) return false;
        outDistance = t;
        return true;
    }

    bool CollisionDetector::CheckRayCapsule(const Ray& ray, const Capsule& capsule, float& outDistance) {
        glm::vec3 axis = capsule.PointB - capsule.PointA;
        float height = glm::length(axis);
        if (height < 0.0001f) {
            return CheckRaySphere(ray, Sphere{ capsule.PointA, capsule.Radius }, outDistance);
        }
        axis /= height;

        glm::vec3 ro = ray.Origin - capsule.PointA;
        float dotDirAxis = glm::dot(ray.Direction, axis);
        float dotRoAxis = glm::dot(ro, axis);

        glm::vec3 d = ray.Direction - axis * dotDirAxis;
        glm::vec3 o = ro - axis * dotRoAxis;
        float a = glm::dot(d, d);
        float b = 2.0f * glm::dot(o, d);
        float c = glm::dot(o, o) - capsule.Radius * capsule.Radius;

        float discriminant = b * b - 4.0f * a * c;
        float tCyl = -1.0f;
        if (discriminant >= 0.0f && a > 0.0001f) {
            float sqrtD = glm::sqrt(discriminant);
            float t1 = (-b - sqrtD) / (2.0f * a);
            float hitHeight1 = dotRoAxis + t1 * dotDirAxis;
            if (t1 >= 0.0f && hitHeight1 >= 0.0f && hitHeight1 <= height) {
                tCyl = t1;
            }
            if (tCyl < 0.0f) {
                float t2 = (-b + sqrtD) / (2.0f * a);
                float hitHeight2 = dotRoAxis + t2 * dotDirAxis;
                if (t2 >= 0.0f && hitHeight2 >= 0.0f && hitHeight2 <= height) {
                    tCyl = t2;
                }
            }
        }

        float tA, tB;
        bool hitA = CheckRaySphere(ray, Sphere{ capsule.PointA, capsule.Radius }, tA);
        bool hitB = CheckRaySphere(ray, Sphere{ capsule.PointB, capsule.Radius }, tB);

        float tSphere = -1.0f;
        if (hitA && hitB) tSphere = glm::min(tA, tB);
        else if (hitA) tSphere = tA;
        else if (hitB) tSphere = tB;

        if (tCyl < 0.0f && tSphere < 0.0f) return false;

        if (tCyl >= 0.0f && tSphere >= 0.0f) {
            outDistance = glm::min(tCyl, tSphere);
        } else if (tCyl >= 0.0f) {
            outDistance = tCyl;
        } else {
            outDistance = tSphere;
        }
        return true;
    }
} // namespace NFSEngine