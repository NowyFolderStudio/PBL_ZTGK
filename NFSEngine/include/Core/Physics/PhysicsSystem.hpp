#pragma once

#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"
#include "Core/Physics/PhysicsPrimitives.hpp"
#include "Core/Physics/CollisionDetector.hpp"
#include "Core/DeltaTime.hpp"
#include <vector>
#include <set>
#include <utility>

namespace NFSEngine {
    class PhysicsSystem {
    public:
        static inline glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        void Update(const std::vector<RigidBody3DComponent*>& rigidBodies, const std::vector<ColliderComponent*>& allColliders,
                    DeltaTime deltaTime);

        void RemoveCollider(ColliderComponent* collider);

        static CollisionInfo CheckCollision(GameObject* a, GameObject* b);

        static AABB GetAABB(Transform* transform, BoxCollider3DComponent* collider);
        static OBB GetOBB(Transform* transform, BoxCollider3DComponent* collider);
        static Sphere GetSphere(Transform* transform, SphereCollider3DComponent* collider);
        static Capsule GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider);
        static Cylinder GetCylinder(Transform* transform, CylinderCollider3DComponent* collider);

    private:
        std::set<std::pair<ColliderComponent*, ColliderComponent*>> m_TriggerPairs;
    };
} // namespace NFSEngine