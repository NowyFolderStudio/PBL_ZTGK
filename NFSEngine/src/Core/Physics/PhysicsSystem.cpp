#include "Core/Physics/PhysicsSystem.hpp"
#include "Core/GameObject.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine {

    CollisionInfo PhysicsSystem::CheckCollision(GameObject* a, GameObject* b) {
        auto* colliderA = a->GetComponent<ColliderComponent>();
        auto* colliderB = b->GetComponent<ColliderComponent>();

        if (!colliderA || !colliderB) return CollisionInfo();

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Box) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckAABB(GetAABB(a->GetTransform(), boxA), GetAABB(b->GetTransform(), boxB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Sphere) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckSphere(GetSphere(a->GetTransform(), sphereA), GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Sphere) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckAABBSphere(GetAABB(a->GetTransform(), boxA), GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Box) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            auto info
                = CollisionDetector::CheckAABBSphere(GetAABB(b->GetTransform(), boxB), GetSphere(a->GetTransform(), sphereA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Capsule) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsule(GetCapsule(a->GetTransform(), capsuleA),
                                                   GetCapsule(b->GetTransform(), capsuleB));
        }

        if (colliderA->Type == ColliderType::Sphere && colliderB->Type == ColliderType::Capsule) {
            auto* sphereA = static_cast<SphereCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info = CollisionDetector::CheckCapsuleSphere(GetCapsule(b->GetTransform(), capsuleB),
                                                              GetSphere(a->GetTransform(), sphereA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Sphere) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* sphereB = static_cast<SphereCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleSphere(GetCapsule(a->GetTransform(), capsuleA),
                                                         GetSphere(b->GetTransform(), sphereB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Box) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* boxB = static_cast<BoxCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleOBB(GetCapsule(a->GetTransform(), capsuleA), GetOBB(b->GetTransform(), boxB));
        }

        if (colliderA->Type == ColliderType::Box && colliderB->Type == ColliderType::Capsule) {
            auto* boxA = static_cast<BoxCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info
                = CollisionDetector::CheckCapsuleOBB(GetCapsule(b->GetTransform(), capsuleB), GetOBB(a->GetTransform(), boxA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Cylinder) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCylinder(GetCylinder(a->GetTransform(), cylinderA),
                                                    GetCylinder(b->GetTransform(), cylinderB));
        }

        if (colliderA->Type == ColliderType::Capsule && colliderB->Type == ColliderType::Cylinder) {
            auto* capsuleA = static_cast<CapsuleCollider3DComponent*>(colliderA);
            auto* cylinderB = static_cast<CylinderCollider3DComponent*>(colliderB);
            return CollisionDetector::CheckCapsuleCylinder(GetCapsule(a->GetTransform(), capsuleA),
                                                           GetCylinder(b->GetTransform(), cylinderB));
        }

        if (colliderA->Type == ColliderType::Cylinder && colliderB->Type == ColliderType::Capsule) {
            auto* cylinderA = static_cast<CylinderCollider3DComponent*>(colliderA);
            auto* capsuleB = static_cast<CapsuleCollider3DComponent*>(colliderB);
            auto info = CollisionDetector::CheckCapsuleCylinder(GetCapsule(b->GetTransform(), capsuleB),
                                                                GetCylinder(a->GetTransform(), cylinderA));
            info.ContactNormal = -info.ContactNormal;
            return info;
        }

        return CollisionInfo();
    }

    void PhysicsSystem::Update(const std::vector<RigidBody3DComponent*>& rigidBodies,
                               const std::vector<ColliderComponent*>& allColliders, DeltaTime deltaTime) {
        float dt = static_cast<float>(deltaTime);

        std::set<std::pair<ColliderComponent*, ColliderComponent*>> currentFrameTriggers;

        for (auto* rigidBody : rigidBodies) {
            GameObject* objA = rigidBody->GetOwner();

            if (!objA->IsActive()) continue;

            auto* colA = objA->GetComponent<ColliderComponent>();
            if (!colA) continue;

            auto* transform = objA->GetTransform();

            if (rigidBody->UseGravity) {
                rigidBody->Acceleration += Gravity;
            }
            rigidBody->Velocity += rigidBody->Acceleration * dt;
            rigidBody->Acceleration = glm::vec3(0.0f);

            glm::vec3 moveDelta = rigidBody->Velocity * dt;

            transform->Move(moveDelta);

            for (auto* colB : allColliders) {
                GameObject* objB = colB->GetOwner();
                if (objA == objB || !objB->IsActive()) continue;

                CollisionInfo info = CheckCollision(objA, objB);

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
                    }
                }
            }
        }

        for (const auto& pair : m_TriggerPairs) {
            if (currentFrameTriggers.find(pair) == currentFrameTriggers.end()) {
                if (pair.first && pair.second) {
                    if (pair.first->OnTriggerExit) pair.first->OnTriggerExit(pair.second->GetOwner());
                    if (pair.second->OnTriggerExit) pair.second->OnTriggerExit(pair.first->GetOwner());
                }
            }
        }

        m_TriggerPairs = std::move(currentFrameTriggers);
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
        box.Min = transform->GetPosition() + collider->Offset - collider->Size * 0.5f;
        box.Max = transform->GetPosition() + collider->Offset + collider->Size * 0.5f;

        return box;
    };

    OBB PhysicsSystem::GetOBB(Transform* transform, BoxCollider3DComponent* collider) {
        OBB obb;

        obb.Center = transform->GetPosition() + collider->Offset;
        obb.HalfSize = collider->Size * 0.5f;
        obb.Rotation = transform->GetRotation();

        return obb;
    };

    Sphere PhysicsSystem::GetSphere(Transform* transform, SphereCollider3DComponent* collider) {
        Sphere sphere;
        sphere.Center = transform->GetPosition() + collider->Offset;
        sphere.Radius = collider->Radius;

        return sphere;
    };

    Capsule PhysicsSystem::GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider) {
        Capsule capsule;

        glm::vec3 halfHeightVec = glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);

        glm::quat rotationQuat = transform->GetRotation();

        glm::vec3 rotatedHalfHeightVec = rotationQuat * halfHeightVec;

        glm::vec3 position = transform->GetPosition() + collider->Offset;

        capsule.PointA = position + rotatedHalfHeightVec;
        capsule.PointB = position - rotatedHalfHeightVec;
        capsule.Radius = collider->Radius;

        return capsule;
    };

    Cylinder PhysicsSystem::GetCylinder(Transform* transform, CylinderCollider3DComponent* collider) {
        Cylinder cylinder;

        cylinder.PointA = transform->GetPosition() + collider->Offset + glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);
        cylinder.PointB = transform->GetPosition() + collider->Offset - glm::vec3(0.0f, collider->Height * 0.5f, 0.0f);

        cylinder.Radius = collider->Radius;

        return cylinder;
    };

} // namespace NFSEngine