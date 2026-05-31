#include "Core/Physics/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"

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

                glm::vec3 pos = col->GetOwner()->GetTransform()->GetWorldPosition();
                GridKey key = GetGridKey(pos);

                m_Grid[key].push_back(col);
            }
        }
        {

            NFS_PROFILE_SCOPE("Physics: Check Collisions & Move");
            for (auto* rigidBody : rigidBodies) {
                GameObject* objA = rigidBody->GetOwner();

                if (!objA->IsActive()) continue;

                auto* colA = objA->GetComponent<ColliderComponent>();
                if (!colA) continue;

                auto* transform = objA->GetTransform();

                rigidBody->IsGrounded = false;
                rigidBody->IsTouchingWall = false;
                rigidBody->WallNormal = glm::vec3(0.0f);
                rigidBody->TouchedWallObject = nullptr;

                if (rigidBody->UseGravity) {
                    rigidBody->Acceleration += Gravity;
                }
                rigidBody->Velocity += rigidBody->Acceleration * dt;
                rigidBody->Acceleration = glm::vec3(0.0f);

                glm::vec3 moveDelta = rigidBody->Velocity * dt;
                transform->Move(moveDelta);

                glm::vec3 myPos = transform->GetWorldPosition();
                GridKey myKey = GetGridKey(myPos);

                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        for (int z = -1; z <= 1; ++z) {

                            GridKey searchKey = { myKey.x + x, myKey.y + y, myKey.z + z };

                            auto it = m_Grid.find(searchKey);
                            if (it != m_Grid.end()) {
                                for (auto* colB : it->second) {
                                    GameObject* objB = colB->GetOwner();
                                    if (objA == objB) continue;

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
                                            transform->Move(info.ContactNormal * info.PenetrationDepth);
                                            float pushback = glm::dot(rigidBody->Velocity, info.ContactNormal);

                                            if (pushback < 0.0f) {
                                                rigidBody->Velocity -= info.ContactNormal * pushback;
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

} // namespace NFSEngine