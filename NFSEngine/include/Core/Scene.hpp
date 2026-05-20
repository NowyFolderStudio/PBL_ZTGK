#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/Physics/PhysicsSystem.hpp"

namespace NFSEngine {

    class DirectionalLight;
    class PointLight;
    class SpotLight;

    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        GameObject* CreateGameObject(const std::string& name = "GameObject");

        void OnUpdate(DeltaTime deltaTime);
        void OnRender();

        DeltaTime GetFixedDeltaTime() { return m_FixedDeltaTime; }
        void SetFixedDeltaTime(DeltaTime fixedDeltaTime) { m_FixedDeltaTime = fixedDeltaTime; }

        const std::vector<std::unique_ptr<GameObject>>& GetAllGameObjects() const { return m_GameObjects; }

        GameObject* FindGameObject(const std::string& name);
        GameObject* FindWithTag(uint32_t tagHash);
        std::vector<GameObject*> FindGameObjectsWithTag(uint32_t tagHash);

        // Light things
        void RegisterDirectionalLight(DirectionalLight* light) { m_CachedDirLight = light; }
        void RegisterPointLight(PointLight* light) { m_CachedPointLights.push_back(light); }
        void RegisterSpotLight(SpotLight* light) { m_CachedSpotLights.push_back(light); }

        void UnregisterDirectionalLight(DirectionalLight* light);
        void UnregisterPointLight(PointLight* light);
        void UnregisterSpotLight(SpotLight* light);

        NFSEngine::DirectionalLight* GetDirLight() const { return m_CachedDirLight; }
        const std::vector<NFSEngine::PointLight*>& GetPointLights() const { return m_CachedPointLights; }
        const std::vector<NFSEngine::SpotLight*>& GetSpotLights() const { return m_CachedSpotLights; }


    private:
        DeltaTime m_FixedDeltaTime = 0.01666f;
        float m_Accumulator = 0.0f;

        std::vector<RigidBody3DComponent*> m_PhysicsBodies;
        std::vector<ColliderComponent*> m_Colliders;
        PhysicsSystem m_PhysicsSystem;

        NFSEngine::DirectionalLight* m_CachedDirLight = nullptr;
        std::vector<NFSEngine::SpotLight*> m_CachedSpotLights;
        std::vector<NFSEngine::PointLight*> m_CachedPointLights;

        std::vector<std::unique_ptr<GameObject>> m_GameObjects;
    };

} // namespace NFSEngine