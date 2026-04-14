#pragma once

#include "Components/PhysicsComponents.hpp"

namespace NFSEngine
{
class PhysicsSystem
{
public:
    void Update(std::vector<std::unique_ptr<GameObject>>& gameObjects, DeltaTime deltaTime);
    glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

private:
    struct AABB
    {
        glm::vec3 Min;
        glm::vec3 Max;
    };

    static bool MathCheckAABB(const AABB& a, const AABB& b);
    static bool CheckCollision(GameObject* a, GameObject* b);
};
}