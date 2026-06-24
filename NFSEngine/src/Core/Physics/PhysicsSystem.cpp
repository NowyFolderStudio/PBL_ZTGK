#include "Core/Physics/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"
#include "Core/Tags.hpp"
#include <unordered_set>

namespace NFSEngine {

    CollisionInfo PhysicsSystem::CheckCollision(ColliderComponent* colliderA, ColliderComponent* colliderB) {

        if (!colliderA || !colliderB) return CollisionInfo();

        Transform* transformA = colliderA->GetOwner()->GetTransform();
        Transform* transformB = colliderB->GetOwner()->GetTransform();

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Box) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckAABB(GetAABB(transformA, boxA), GetAABB(transformB, boxB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Sphere) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckSphere(GetSphere(transformA, sphereA), GetSphere(transformB, sphereB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Sphere) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckAABBSphere(GetAABB(transformA, boxA), GetSphere(transformB, sphereB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Box) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            auto info = CollisionDetector::CheckAABBSphere(GetAABB(transformB, boxB), GetSphere(transformA, sphereA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Capsule) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsule(GetCapsule(transformA, capsuleA), GetCapsule(transformB, capsuleB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Capsule) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info = CollisionDetector::CheckCapsuleSphere(GetCapsule(transformB, capsuleB), GetSphere(transformA, sphereA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Sphere) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleSphere(GetCapsule(transformA, capsuleA), GetSphere(transformB, sphereB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Box) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleOBB(GetCapsule(transformA, capsuleA), GetOBB(transformB, boxB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Capsule) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info = CollisionDetector::CheckCapsuleOBB(GetCapsule(transformB, capsuleB), GetOBB(transformA, boxA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Cylinder) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCylinder(GetCylinder(transformA, cylinderA), GetCylinder(transformB, cylinderB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Cylinder) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleCylinder(GetCapsule(transformA, capsuleA), GetCylinder(transformB, cylinderB));
        }

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Capsule) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info
                = CollisionDetector::CheckCapsuleCylinder(GetCapsule(transformB, capsuleB), GetCylinder(transformA, cylinderA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        return CollisionInfo();
    }

    void PhysicsSystem::Update(const std::vector<RigidBody3DComponent*>& rigidBodies,
                               const std::vector<ColliderComponent*>& allColliders, DeltaTime deltaTime) {
        NFS_PROFILE_FUNCTION();
        float dt = static_cast<float>(deltaTime);

        for (auto* col : allColliders) {
            if (col->GetOwner()->IsActive()) {
                col->GetOwner()->GetTransform()->SavePreviousWorldPosition();
            }
        }

        std::set<std::pair<ColliderComponent*, ColliderComponent*>> currentFrameTriggers;
        {
            NFS_PROFILE_SCOPE("Physics: Clear Grid");
            for (auto& [key, colliders] : m_Grid) {
                colliders.clear();
            }
        }

        {
            NFS_PROFILE_SCOPE("Physics: Populate Grid");
            for (auto* col : allColliders) {
                if (!col->GetOwner()->IsActive()) continue;

                AABB box = GetColliderBounds(col);

                GridKey minKey = GetGridKey(box.Min);
                GridKey maxKey = GetGridKey(box.Max);

                for (int x = minKey.x; x <= maxKey.x; ++x) {
                    for (int y = minKey.y; y <= maxKey.y; ++y) {
                        for (int z = minKey.z; z <= maxKey.z; ++z) {
                            GridKey key = { x, y, z };
                            m_Grid[key].push_back(col);
                        }
                    }
                }
            }
        }
        {

            NFS_PROFILE_SCOPE("Physics: Check Collisions & Move");
            for (auto* rigidBody : rigidBodies) {
                rigidBody->PreviousVelocity = rigidBody->Velocity;
                GameObject* objA = rigidBody->GetOwner();

                if (!objA->IsActive()) continue;

                auto* colA = objA->GetComponent<ColliderComponent>();
                if (!colA) continue;

                auto* transform = objA->GetTransform();

                rigidBody->IsGrounded = false;
                rigidBody->IsTouchingWall = false;
                rigidBody->WallNormal = glm::vec3(0.0f);
                rigidBody->TouchedWallObject = nullptr;

                if (!rigidBody->IsKinematic) {
                    if (rigidBody->UseGravity) {
                        rigidBody->Acceleration += Gravity;
                    }
                } else {
                    if (glm::length(rigidBody->AngularVelocity) > 0.0001f) {
                        transform->Rotate(rigidBody->AngularVelocity * dt);
                    }
                }
                rigidBody->Velocity += rigidBody->Acceleration * dt;
                rigidBody->Acceleration = glm::vec3(0.0f);

                glm::vec3 moveDelta = rigidBody->Velocity * dt;
                transform->Move(moveDelta);

                glm::vec3 myPos = transform->GetWorldPosition();
                GridKey myKey = GetGridKey(myPos);

                std::unordered_set<ColliderComponent*> checkedColliders;

                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        for (int z = -1; z <= 1; ++z) {

                            GridKey searchKey = { myKey.x + x, myKey.y + y, myKey.z + z };

                            auto it = m_Grid.find(searchKey);
                            if (it != m_Grid.end()) {
                                for (auto* colB : it->second) {
                                    GameObject* objB = colB->GetOwner();
                                    if (objA == objB) continue;

                                    if (!checkedColliders.insert(colB).second) {
                                        continue;
                                    }

                                    CollisionInfo info = CheckCollision(colA, colB);

                                    if (info.IsColliding) {

                                        if (colA->IsTrigger || colB->IsTrigger) {

                                            auto pair = (colA < colB) ? std::make_pair(colA, colB) : std::make_pair(colB, colA);
                                            currentFrameTriggers.insert(pair);

                                            bool isNewCollision = m_TriggerPairs.find(pair) == m_TriggerPairs.end();

                                            if (isNewCollision) {
                                                if (colA->OnTriggerEnter) colA->OnTriggerEnter(objB);
                                                if (colB->OnTriggerEnter) colB->OnTriggerEnter(objA);
                                            } else {
                                                if (colA->OnTriggerStay) colA->OnTriggerStay(objB);
                                                if (colB->OnTriggerStay) colB->OnTriggerStay(objA);
                                            }
                                        } else {
                                            if (!rigidBody->IsKinematic) {
                                                glm::vec3 worldPos = transform->GetWorldPosition();
                                                transform->SetWorldPosition(worldPos + info.ContactNormal * info.PenetrationDepth);

                                                glm::vec3 velB = glm::vec3(0.0f);
                                                if (auto* rbB = objB->GetComponent<RigidBody3DComponent>()) {
                                                    velB = rbB->Velocity;
                                                }

                                                glm::vec3 relativeVelocity = rigidBody->Velocity - velB;
                                                float velAlongNormal = glm::dot(relativeVelocity, info.ContactNormal);

                                                if (velAlongNormal < 0.0f) {
                                                    float restitution = 0.0f;
                                                    float j = -(1.0f + restitution) * velAlongNormal;

                                                    float maxImpulse = 50.0f;
                                                    if (j > maxImpulse) {
                                                        j = maxImpulse;
                                                    }

                                                    glm::vec3 impulseVector = info.ContactNormal * j;

                                                    rigidBody->Velocity += impulseVector;
                                                }
                                            }

                                            if (colA->OnCollisionEnter) colA->OnCollisionEnter(objB, info.ContactNormal);
                                            if (colB->OnCollisionEnter) colB->OnCollisionEnter(objA, -info.ContactNormal);

                                            if (info.ContactNormal.y > 0.7f) {
                                                rigidBody->IsGrounded = true;
                                                rigidBody->TouchedFloorObject = objB; // TODO: Rework to use one object
                                                rigidBody->FloorNormal = info.ContactNormal;

                                            } else if (std::abs(info.ContactNormal.y) < 0.3f) {
                                                rigidBody->IsTouchingWall = true;
                                                rigidBody->WallNormal = info.ContactNormal;
                                                rigidBody->TouchedWallObject = objB;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            NFS_PROFILE_SCOPE("Physics: Triggers Update");
            for (const auto& pair : m_TriggerPairs) {
                if (currentFrameTriggers.find(pair) == currentFrameTriggers.end()) {
                    if (pair.first && pair.second) {
                        if (pair.first->OnTriggerExit) pair.first->OnTriggerExit(pair.second->GetOwner());
                        if (pair.second->OnTriggerExit) pair.second->OnTriggerExit(pair.first->GetOwner());
                    }
                }
            }

            m_TriggerPairs = std::move(currentFrameTriggers);
        }
    };

    void PhysicsSystem::RemoveCollider(ColliderComponent* collider) {
        if (!collider) return;

        for (auto it = m_TriggerPairs.begin(); it != m_TriggerPairs.end();) {
            if (it->first == collider || it->second == collider) {

                if (it->first == collider && it->second->OnTriggerExit) {
                    it->second->OnTriggerExit(it->first->GetOwner());
                } else if (it->second == collider && it->first->OnTriggerExit) {
                    it->first->OnTriggerExit(it->second->GetOwner());
                }

                it = m_TriggerPairs.erase(it);
            } else {
                ++it;
            }
        }
    };

    AABB PhysicsSystem::GetAABB(Transform* transform, BoxCollider3DComponent* collider) {
        AABB box;
        glm::vec3 worldScale = transform->GetWorldScale();
        glm::vec3 rotatedOffset = transform->GetWorldRotation() * (collider->Offset * worldScale);
        glm::vec3 scaledSize = collider->Size * worldScale;

        box.Min = transform->GetWorldPosition() + rotatedOffset - scaledSize * 0.5f;
        box.Max = transform->GetWorldPosition() + rotatedOffset + scaledSize * 0.5f;

        return box;
    };

    OBB PhysicsSystem::GetOBB(Transform* transform, BoxCollider3DComponent* collider) {
        OBB obb;
        glm::vec3 worldScale = transform->GetWorldScale();
        glm::quat worldRot = transform->GetWorldRotation();

        glm::vec3 rotatedOffset = worldRot * (collider->Offset * worldScale);

        obb.Center = transform->GetWorldPosition() + rotatedOffset;
        obb.HalfSize = (collider->Size * worldScale) * 0.5f;
        obb.Rotation = worldRot;

        return obb;
    }

    Sphere PhysicsSystem::GetSphere(Transform* transform, SphereCollider3DComponent* collider) {
        Sphere sphere;
        glm::vec3 worldScale = transform->GetWorldScale();

        float maxScale = std::max({ std::abs(worldScale.x), std::abs(worldScale.y), std::abs(worldScale.z) });

        glm::vec3 rotatedOffset = transform->GetWorldRotation() * (collider->Offset * worldScale);

        sphere.Center = transform->GetWorldPosition() + rotatedOffset;
        sphere.Radius = collider->Radius * maxScale;

        return sphere;
    }

    Capsule PhysicsSystem::GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider) {
        Capsule capsule;
        glm::vec3 worldScale = transform->GetWorldScale();
        glm::quat worldRot = transform->GetWorldRotation();

        float maxRadiusScale = std::max(std::abs(worldScale.x), std::abs(worldScale.z));
        float scaledHeight = collider->Height * std::abs(worldScale.y);

        glm::vec3 rotatedOffset = worldRot * (collider->Offset * worldScale);
        glm::vec3 position = transform->GetWorldPosition() + rotatedOffset;

        glm::vec3 halfHeightVec = glm::vec3(0.0f, scaledHeight * 0.5f, 0.0f);
        glm::vec3 rotatedHalfHeightVec = worldRot * halfHeightVec;

        capsule.PointA = position + rotatedHalfHeightVec;
        capsule.PointB = position - rotatedHalfHeightVec;
        capsule.Radius = collider->Radius * maxRadiusScale;

        return capsule;
    }

    Cylinder PhysicsSystem::GetCylinder(Transform* transform, CylinderCollider3DComponent* collider) {
        Cylinder cylinder;
        glm::vec3 worldScale = transform->GetWorldScale();
        glm::quat worldRot = transform->GetWorldRotation();

        float maxRadiusScale = std::max(std::abs(worldScale.x), std::abs(worldScale.z));
        float scaledHeight = collider->Height * std::abs(worldScale.y);

        glm::vec3 rotatedOffset = worldRot * (collider->Offset * worldScale);
        glm::vec3 position = transform->GetWorldPosition() + rotatedOffset;

        glm::vec3 halfHeightVec = glm::vec3(0.0f, scaledHeight * 0.5f, 0.0f);

        glm::vec3 rotatedHalfHeightVec = worldRot * halfHeightVec;

        cylinder.PointA = position + rotatedHalfHeightVec;
        cylinder.PointB = position - rotatedHalfHeightVec;
        cylinder.Radius = collider->Radius * maxRadiusScale;

        return cylinder;
    }

    AABB PhysicsSystem::GetColliderBounds(ColliderComponent* col) {
        Transform* transform = col->GetOwner()->GetTransform();

        switch (col->Type) {
        case ColliderType::Box:
            return GetAABB(transform, static_cast<BoxCollider3DComponent*>(col));

        case ColliderType::Sphere: {
            auto sphere = GetSphere(transform, static_cast<SphereCollider3DComponent*>(col));
            return { sphere.Center - glm::vec3(sphere.Radius), sphere.Center + glm::vec3(sphere.Radius) };
        }

        case ColliderType::Capsule: {
            auto capsule = GetCapsule(transform, static_cast<CapsuleCollider3DComponent*>(col));
            glm::vec3 radiusVec(capsule.Radius);
            glm::vec3 minP = glm::min(capsule.PointA, capsule.PointB) - radiusVec;
            glm::vec3 maxP = glm::max(capsule.PointA, capsule.PointB) + radiusVec;
            return { minP, maxP };
        }

        case ColliderType::Cylinder: {
            auto cylinder = GetCylinder(transform, static_cast<CylinderCollider3DComponent*>(col));

            glm::vec3 axis = cylinder.PointB - cylinder.PointA;
            float height = glm::length(axis);

            if (height < 0.0001f) {
                glm::vec3 radiusVec(cylinder.Radius);
                return { cylinder.PointA - radiusVec, cylinder.PointA + radiusVec };
            }

            glm::vec3 dir = axis / height;

            glm::vec3 extent;
            extent.x = (std::abs(axis.x) * 0.5f) + cylinder.Radius * std::sqrt(std::max(0.0f, 1.0f - dir.x * dir.x));
            extent.y = (std::abs(axis.y) * 0.5f) + cylinder.Radius * std::sqrt(std::max(0.0f, 1.0f - dir.y * dir.y));
            extent.z = (std::abs(axis.z) * 0.5f) + cylinder.Radius * std::sqrt(std::max(0.0f, 1.0f - dir.z * dir.z));

            glm::vec3 center = (cylinder.PointA + cylinder.PointB) * 0.5f;

            return { center - extent, center + extent };
        }
        }

        return { glm::vec3(0.0f), glm::vec3(0.0f) };
    }

    bool PhysicsSystem::RaycastCollider(const Ray& ray, float maxDistance, ColliderComponent* collider, Transform* transform,
                                        RaycastResult& outResult) {
        float hitDist;

        switch (collider->Type) {
        case ColliderType::Box: {
            auto* box = static_cast<BoxCollider3DComponent*>(collider);
            if (CollisionDetector::CheckRayOBB(ray, GetOBB(transform, box), hitDist)) {
                if (hitDist <= maxDistance) {
                    outResult.Hit = true;
                    outResult.Distance = hitDist;
                    outResult.Point = ray.Origin + ray.Direction * hitDist;
                    outResult.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    return true;
                }
            }
            break;
        }
        case ColliderType::Sphere: {
            auto* sphereCollider = static_cast<SphereCollider3DComponent*>(collider);
            if (CollisionDetector::CheckRaySphere(ray, GetSphere(transform, sphereCollider), hitDist)) {
                if (hitDist <= maxDistance) {
                    outResult.Hit = true;
                    outResult.Distance = hitDist;
                    outResult.Point = ray.Origin + ray.Direction * hitDist;
                    glm::vec3 sphereCenter = GetSphere(transform, sphereCollider).Center;
                    outResult.Normal = glm::normalize(outResult.Point - sphereCenter);
                    return true;
                }
            }
            break;
        }
        case ColliderType::Capsule: {
            auto* capsuleCollider = static_cast<CapsuleCollider3DComponent*>(collider);
            if (CollisionDetector::CheckRayCapsule(ray, GetCapsule(transform, capsuleCollider), hitDist)) {
                if (hitDist <= maxDistance) {
                    outResult.Hit = true;
                    outResult.Distance = hitDist;
                    outResult.Point = ray.Origin + ray.Direction * hitDist;
                    glm::vec3 capsuleCenter
                        = (GetCapsule(transform, capsuleCollider).PointA + GetCapsule(transform, capsuleCollider).PointB) * 0.5f;
                    outResult.Normal = glm::normalize(outResult.Point - capsuleCenter);
                    return true;
                }
            }
            break;
        }
        case ColliderType::Cylinder: {
            auto* cylinderCollider = static_cast<CylinderCollider3DComponent*>(collider);
            Cylinder cyl = GetCylinder(transform, cylinderCollider);

            if (CollisionDetector::CheckRayCylinder(ray, cyl, hitDist)) {
                if (hitDist <= maxDistance) {
                    outResult.Hit = true;
                    outResult.Distance = hitDist;
                    outResult.Point = ray.Origin + ray.Direction * hitDist;

                    glm::vec3 axis = cyl.PointB - cyl.PointA;
                    float height = glm::length(axis);

                    if (height > 0.0001f) {
                        axis /= height;
                        glm::vec3 hitVector = outResult.Point - cyl.PointA;
                        float hitHeight = glm::dot(hitVector, axis);

                        const float epsilon = 0.001f;

                        if (hitHeight <= epsilon) {
                            outResult.Normal = -axis;
                        } else if (hitHeight >= height - epsilon) {
                            outResult.Normal = axis;
                        } else {
                            glm::vec3 closestAxisPoint = cyl.PointA + axis * hitHeight;
                            outResult.Normal = glm::normalize(outResult.Point - closestAxisPoint);
                        }
                    } else {
                        outResult.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    return true;
                }
            }
            break;
        }
        default:
            break;
        }

        return false;
    }

    bool PhysicsSystem::Raycast(const Ray& ray, RaycastResult& outResult, const std::vector<ColliderComponent*>& allColliders,
                                const RaycastOptions& options) {
        bool hitAnything = false;
        float closestDistance = options.MaxDistance;
        RaycastResult closestResult;

        for (auto* collider : allColliders) {
            GameObject* go = collider->GetOwner();
            if (!go->IsActive()) continue;

            if (options.IgnoreTriggers && collider->IsTrigger) continue;

            if (options.TagsToIgnore != 0) {
                Transform* ancestor = go->GetTransform();
                bool isIgnoredOrDescendant = false;
                while (ancestor) {
                    if (ancestor->GetOwner()->CompareTag(options.TagsToIgnore)) {
                        isIgnoredOrDescendant = true;
                        break;
                    }
                    ancestor = ancestor->GetParent();
                }
                if (isIgnoredOrDescendant) continue;
            }

            RaycastResult tempResult;
            if (RaycastCollider(ray, closestDistance, collider, go->GetTransform(), tempResult)) {
                if (tempResult.Distance < closestDistance) {
                    closestDistance = tempResult.Distance;
                    closestResult = tempResult;
                    hitAnything = true;
                }
            }
        }

        if (hitAnything) {
            outResult = closestResult;
            return true;
        }

        return false;
    }

} // namespace NFSEngine