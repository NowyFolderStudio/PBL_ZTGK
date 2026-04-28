#include "Core/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine {

    glm::vec3 PhysicsSystem::ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& point) {
        glm::vec3 ab = b - a;
        float lengthSquared = glm::dot(ab, ab);

        if (lengthSquared == 0.0f) return a;
        float t = glm::dot(point - a, ab) / lengthSquared;

        return a + glm::clamp(t, 0.0f, 1.0f) * ab;
    }

    glm::vec3 PhysicsSystem::ClampPointToAABB(const glm::vec3& point, const AABB& box) {
        return glm::vec3(std::max(box.Min.x, std::min(point.x, box.Max.x)), std::max(box.Min.y, std::min(point.y, box.Max.y)),
                         std::max(box.Min.z, std::min(point.z, box.Max.z)));
    }

    glm::vec3 PhysicsSystem::ClampPointToCylinder(const glm::vec3& point, const Cylinder& cyl) {
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

    float PhysicsSystem::SqDistSegToSeg(const glm::vec3& p1, const glm::vec3& q1, const glm::vec3& p2, const glm::vec3& q2) {
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

    bool PhysicsSystem::MathCheckAABB(const AABB& a, const AABB& b) {
        return (a.Min.x <= b.Max.x && a.Max.x >= b.Min.x) && (a.Min.y <= b.Max.y && a.Max.y >= b.Min.y)
            && (a.Min.z <= b.Max.z && a.Max.z >= b.Min.z);
    };

    bool PhysicsSystem::MathCheckSphere(const Sphere& a, const Sphere& b) {
        glm::vec3 diff = a.Center - b.Center;

        float distanceSquared = glm::dot(diff, diff);
        float radiusSum = a.Radius + b.Radius;

        return distanceSquared < (radiusSum * radiusSum);
    }

    bool PhysicsSystem::MathCheckAABBSphere(const AABB& box, const Sphere& sphere) {
        glm::vec3 closestPoint = ClampPointToAABB(sphere.Center, box);
        glm::vec3 diff = closestPoint - sphere.Center;

        float distanceSquared = glm::dot(diff, diff);

        return distanceSquared < (sphere.Radius * sphere.Radius);
    }

    bool PhysicsSystem::MathCheckCapsule(const Capsule& a, const Capsule& b) {
        float distanceSquared = SqDistSegToSeg(a.PointA, a.PointB, b.PointA, b.PointB);
        float radiusSum = a.Radius + b.Radius;

        return distanceSquared < (radiusSum * radiusSum);
    }

    bool PhysicsSystem::MathCheckCapsuleAABB(const Capsule& capsule, const AABB& box) {
        glm::vec3 aabbCenter = (box.Min + box.Max) * 0.5f;
        glm::vec3 closestOnSegment = ClosestPointOnSegment(capsule.PointA, capsule.PointB, aabbCenter);
        glm::vec3 closestOnAABB = ClampPointToAABB(closestOnSegment, box);
        glm::vec3 finalClosestPoint = ClosestPointOnSegment(capsule.PointA, capsule.PointB, closestOnAABB);
        glm::vec3 diff = finalClosestPoint - closestOnAABB;

        float distanceSquared = glm::dot(diff, diff);

        return distanceSquared < (capsule.Radius * capsule.Radius);
    }

    bool PhysicsSystem::MathCheckCapsuleCylinder(const Capsule& capsule, const Cylinder& cylinder) {
        glm::vec3 cylCenter = (cylinder.PointA + cylinder.PointB) * 0.5f;

        glm::vec3 closestOnSegment = ClosestPointOnSegment(capsule.PointA, capsule.PointB, cylCenter);
        glm::vec3 closestOnCylinder = ClampPointToCylinder(closestOnSegment, cylinder);
        glm::vec3 finalClosestPoint = ClosestPointOnSegment(capsule.PointA, capsule.PointB, closestOnCylinder);
        glm::vec3 diff = finalClosestPoint - closestOnCylinder;

        float distanceSquared = glm::dot(diff, diff);

        return distanceSquared < (capsule.Radius * capsule.Radius);
    }

    bool PhysicsSystem::MathCheckCapsuleSphere(const Capsule& capsule, const Sphere& sphere) {
        glm::vec3 closestOnSegment = ClosestPointOnSegment(capsule.PointA, capsule.PointB, sphere.Center);
        glm::vec3 diff = sphere.Center - closestOnSegment;

        float distanceSquared = glm::dot(diff, diff);
        float radiusSum = capsule.Radius + sphere.Radius;

        return distanceSquared < (radiusSum * radiusSum);
    }

    bool PhysicsSystem::MathCheckCylinder(const Cylinder& a, const Cylinder& b) { // TODO: Check if this needs to be refactored
        float aMinY = std::min(a.PointA.y, a.PointB.y);
        float aMaxY = std::max(a.PointA.y, a.PointB.y);
        float bMinY = std::min(b.PointA.y, b.PointB.y);
        float bMaxY = std::max(b.PointA.y, b.PointB.y);

        if (aMaxY <= bMinY || aMinY >= bMaxY) {
            return false;
        }

        glm::vec2 aPos2D = glm::vec2(a.PointA.x, a.PointA.z);
        glm::vec2 bPos2D = glm::vec2(b.PointA.x, b.PointA.z);
        glm::vec2 diff = aPos2D - bPos2D;

        float distanceSquared = glm::dot(diff, diff);
        float radiusSum = a.Radius + b.Radius;

        return distanceSquared < (radiusSum * radiusSum);
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

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Cylinder) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);

            return MathCheckCylinder(GetCylinder(a->GetTransform(), cylinderA), GetCylinder(b->GetTransform(), cylinderB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Cylinder) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);

            return MathCheckCapsuleCylinder(GetCapsule(a->GetTransform(), capsuleA), GetCylinder(b->GetTransform(), cylinderB));
        }

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Capsule) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);

            return MathCheckCapsuleCylinder(GetCapsule(b->GetTransform(), capsuleB), GetCylinder(a->GetTransform(), cylinderA));
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

        capsule.PointA = transform->GetPosition() + collider->Offset + glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);
        capsule.PointB = transform->GetPosition() + collider->Offset - glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);

        capsule.Radius = collider->Radius;

        return capsule;
    };

    PhysicsSystem::Cylinder PhysicsSystem::GetCylinder(Transform* transform, CylinderCollider3DComponent* collider) {
        PhysicsSystem::Cylinder cylinder;

        cylinder.PointA = transform->GetPosition() + collider->Offset + glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);
        cylinder.PointB = transform->GetPosition() + collider->Offset - glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);

        cylinder.Radius = collider->Radius;

        return cylinder;
    };

} // namespace NFSEngine