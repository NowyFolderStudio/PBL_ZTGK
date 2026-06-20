#pragma once

#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"
#include "Core/Physics/PhysicsPrimitives.hpp"
#include "Core/Physics/CollisionDetector.hpp"
#include "Core/Tags.hpp"
#include "Core/DeltaTime.hpp"
#include <vector>
#include <set>
#include <utility>

namespace NFSEngine {

    struct GridKey {
        int x, y, z;

        bool operator==(const GridKey& other) const { return x == other.x && y == other.y && z == other.z; }
    };

    struct GridKeyHash {
        std::size_t operator()(const GridKey& key) const {
            return ((std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1)) >> 1) ^ (std::hash<int>()(key.z) << 1);
        }
    };

    struct RaycastOptions {
        float MaxDistance = 1000.0f;
        bool IgnoreTriggers = true;
        uint32_t TagsToIgnore = Tags::Untagged;
    };

    class PhysicsSystem {
    public:
        static inline glm::vec3 Gravity = glm::vec3(0.0f, -25.0f, 0.0f);

        void Update(const std::vector<RigidBody3DComponent*>& rigidBodies, const std::vector<ColliderComponent*>& allColliders,
                    DeltaTime deltaTime);

        void RemoveCollider(ColliderComponent* collider);

        static CollisionInfo CheckCollision(ColliderComponent* colliderA, ColliderComponent* colliderB);

        static AABB GetAABB(Transform* transform, BoxCollider3DComponent* collider);
        static OBB GetOBB(Transform* transform, BoxCollider3DComponent* collider);
        static Sphere GetSphere(Transform* transform, SphereCollider3DComponent* collider);
        static Capsule GetCapsule(Transform* transform, CapsuleCollider3DComponent* collider);
        static Cylinder GetCylinder(Transform* transform, CylinderCollider3DComponent* collider);
        static AABB GetColliderBounds(ColliderComponent* col);
        static bool RaycastCollider(const Ray& ray, float maxDistance, ColliderComponent* collider, Transform* transform,
                                    RaycastResult& outResult);
        static bool Raycast(const Ray& ray, RaycastResult& outResult, const std::vector<ColliderComponent*>& allColliders,
                            const RaycastOptions& options = RaycastOptions());

    private:
        static constexpr float CELL_SIZE = 20.0f;

        using SpatialGrid = std::unordered_map<GridKey, std::vector<ColliderComponent*>, GridKeyHash>;
        SpatialGrid m_Grid;

        GridKey GetGridKey(const glm::vec3& position) const {
            return { static_cast<int>(std::floor(position.x / CELL_SIZE)), static_cast<int>(std::floor(position.y / CELL_SIZE)),
                     static_cast<int>(std::floor(position.z / CELL_SIZE)) };
        }

        std::set<std::pair<ColliderComponent*, ColliderComponent*>> m_TriggerPairs;
    };
} // namespace NFSEngine