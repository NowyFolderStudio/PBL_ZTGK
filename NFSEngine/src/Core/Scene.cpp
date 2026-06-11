#include "Core/CullingUtils.hpp"
#include "Core/Scene.hpp"
#include "Debug/Profiler.hpp"
#include "Renderer/Renderer.hpp"
#include <algorithm>

#include "Components/DirectionalLight.hpp"
#include "Components/PointLight.hpp"
#include "Components/SpotLight.hpp"

#include "Components/CubeMesh.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/ParticleComponent.hpp"

namespace NFSEngine {

    GameObject* Scene::CreateGameObject(const std::string& name) {
        auto go = std::make_unique<GameObject>(this, name);
        GameObject* rawPtr = go.get();
        m_GameObjects.push_back(std::move(go));

        m_RenderablesDirty = true;

        return rawPtr;
    }

    GameObject* Scene::FindGameObject(const std::string& name) {
        for (auto& go : m_GameObjects) {
            if (go->name == name) {
                return go.get();
            }
        }
        return nullptr;
    }

    void Scene::OnUpdate(DeltaTime deltaTime) {
        NFS_PROFILE_FUNCTION();

        {
            NFS_PROFILE_SCOPE("Scene: Awake & Start");
            for (size_t i = 0; i < m_GameObjects.size(); i++) {
                auto& gameObject = m_GameObjects[i];
                gameObject->Awake();
                gameObject->Start();
            }
        }
        {
            NFS_PROFILE_SCOPE("Scene: Physics Prep (Vectors)");
            if (m_PhysicsListsDirty) {
                m_PhysicsBodies.clear();
                m_Colliders.clear();

                for (const auto& gameObject : m_GameObjects) {
                    if (!gameObject->IsActive()) continue;

                    if (auto* rb = gameObject->GetComponent<RigidBody3DComponent>()) {
                        m_PhysicsBodies.push_back(rb);
                    }
                    if (auto* col = gameObject->GetComponent<ColliderComponent>()) {
                        m_Colliders.push_back(col);
                    }
                }

                m_PhysicsListsDirty = false;
            }
        }
        if (m_RenderablesDirty) {
            m_Renderables.clear();
            for (const auto& gameObject : m_GameObjects) {
                if (!gameObject->IsActive()) continue;

                if (gameObject->HasComponent<ModelComponent>() || gameObject->HasComponent<CubeMesh>()
                    || gameObject->HasComponent<ParticleComponent>()) {
                    m_Renderables.push_back(gameObject.get());
                }
            }
            m_RenderablesDirty = false;
        }

        if (deltaTime > 0.25f) deltaTime = 0.25f;
        m_Accumulator += deltaTime;

        {
            NFS_PROFILE_SCOPE("Scene: Fixed Update & Physics");
            while (m_Accumulator >= m_FixedDeltaTime) {
                for (size_t i = 0; i < m_GameObjects.size(); i++) {
                    auto& gameObject = m_GameObjects[i];
                    gameObject->FixedUpdate(m_FixedDeltaTime);
                }
                m_PhysicsSystem.Update(m_PhysicsBodies, m_Colliders, m_FixedDeltaTime);
                m_Accumulator -= m_FixedDeltaTime;
            }
        }

        {
            NFS_PROFILE_SCOPE("Scene: Main GameObjects Update");
            for (size_t i = 0; i < m_GameObjects.size(); i++) {
                auto& gameObject = m_GameObjects[i];
                gameObject->Update(deltaTime);
            }
        }

        {
            NFS_PROFILE_SCOPE("Scene: Garbage Collection");
            m_GameObjects.erase(std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
                                               [this](const std::unique_ptr<GameObject>& gameObject) {
                                                   if (gameObject->IsDestroyed()) {

                                                       bool hadPhysics = false;

                                                       if (auto* col = gameObject->GetComponent<ColliderComponent>()) {
                                                           m_PhysicsSystem.RemoveCollider(col);
                                                           hadPhysics = true;
                                                       }
                                                       if (gameObject->GetComponent<RigidBody3DComponent>()) {
                                                           hadPhysics = true;
                                                       }

                                                       if (hadPhysics) {
                                                           m_PhysicsListsDirty = true;
                                                       }

                                                       m_RenderablesDirty = true;

                                                       return true;
                                                   }
                                                   return false;
                                               }),
                                m_GameObjects.end());
        }
    }

    void Scene::OnRender() {
        NFS_PROFILE_FUNCTION();

        const Frustum& frustum = Renderer::GetFrustum();
        float cullingRange = Renderer::GetCullingRange();
        bool cullingEnabled = Renderer::IsFrustumCullingEnabled();
        int cullingMode = Renderer::GetFrustumCullingMode();

        // ITERUJEMY TYLKO PO OBIEKTACH GRAFICZNYCH!
        for (auto* gameObject : m_Renderables) {
            if (!gameObject->IsActive()) continue;

            Transform* transform = gameObject->GetTransform();

            if (cullingEnabled && transform) {
                bool visible = true;

                // --- ZOPTYMALIZOWANY CULLING ---
                if (cullingMode == 0) { // Sfera
                    // UWAGA: Pobieramy wcześniej zcache'owaną sferę, NIE LICZYMY JEJ OD NOWA!
                    // Wymaga dopisania GetCachedLocalSphere() w Twojej klasie renderującej
                    BoundingSphere localSphere;
                    if (auto* mc = gameObject->GetComponent<ModelComponent>())
                        localSphere = mc->GetCachedLocalSphere();
                    else if (auto* cm = gameObject->GetComponent<CubeMesh>())
                        localSphere = cm->GetCachedLocalSphere();

                    if (localSphere.Radius > 0.0f) {
                        // Reszta logiki wyliczania sfery jest OK, bo to tylko jedna macierz i wektor
                        glm::mat4 global = transform->GetGlobalMatrix();
                        glm::vec4 worldCenter = global * glm::vec4(localSphere.Center, 1.0f);

                        float scaleX = glm::length(glm::vec3(global[0]));
                        float scaleY = glm::length(glm::vec3(global[1]));
                        float scaleZ = glm::length(glm::vec3(global[2]));
                        float maxScale = std::max({ scaleX, scaleY, scaleZ });

                        BoundingSphere sphere = { glm::vec3(worldCenter), localSphere.Radius * maxScale };

                        if (!frustum.TestSphere(sphere)) visible = false;
                        if (visible && cullingRange > 0.0f) {
                            float dist = glm::distance(sphere.Center, Renderer::GetCameraPosition());
                            if (dist - sphere.Radius > cullingRange) visible = false;
                        }
                    }
                } else { // AABB
                    // UWAGA: Pobieramy wcześniej zcache'owane AABB
                    std::pair<glm::vec3, glm::vec3> localAABB;
                    if (auto* mc = gameObject->GetComponent<ModelComponent>())
                        localAABB = mc->GetCachedLocalAABB();
                    else if (auto* cm = gameObject->GetComponent<CubeMesh>())
                        localAABB = cm->GetCachedLocalAABB();

                    bool hasBounds = localAABB.first != localAABB.second;

                    if (hasBounds) {
                        glm::mat4 global = transform->GetGlobalMatrix();
                        glm::vec3 corners[8] = {
                            { localAABB.first.x, localAABB.first.y, localAABB.first.z },
                            { localAABB.first.x, localAABB.first.y, localAABB.second.z },
                            { localAABB.first.x, localAABB.second.y, localAABB.first.z },
                            { localAABB.first.x, localAABB.second.y, localAABB.second.z },
                            { localAABB.second.x, localAABB.first.y, localAABB.first.z },
                            { localAABB.second.x, localAABB.first.y, localAABB.second.z },
                            { localAABB.second.x, localAABB.second.y, localAABB.first.z },
                            { localAABB.second.x, localAABB.second.y, localAABB.second.z },
                        };
                        glm::vec3 worldMin(1e10f), worldMax(-1e10f);
                        for (auto& c : corners) {
                            // Idealnie byłoby to przenieść do Transform i aktualizować tylko gdy isDirty == true
                            glm::vec3 w = glm::vec3(global * glm::vec4(c, 1.0f));
                            worldMin = glm::min(worldMin, w);
                            worldMax = glm::max(worldMax, w);
                        }

                        if (!frustum.TestAABB(worldMin, worldMax)) visible = false;
                        if (visible && cullingRange > 0.0f) {
                            glm::vec3 center = (worldMin + worldMax) * 0.5f;
                            float radius = glm::length(worldMax - center);
                            float dist = glm::distance(center, Renderer::GetCameraPosition());
                            if (dist - radius > cullingRange) visible = false;
                        }
                    }
                }

                if (!visible) continue;
            }

            // Odpalamy Render tylko na obiektach, o których już WIEMY, że mają co rysować
            gameObject->Render();
        }
    }

    GameObject* Scene::FindWithTag(uint32_t tagHash) {
        for (auto& go : m_GameObjects) {
            if (go->CompareTag(tagHash)) {
                return go.get();
            }
        }
        return nullptr;
    }

    std::vector<GameObject*> Scene::FindGameObjectsWithTag(uint32_t tagHash) {
        std::vector<GameObject*> result;
        for (auto& go : m_GameObjects) {
            if (go->CompareTag(tagHash)) {
                result.push_back(go.get());
            }
        }
        return result;
    }

    void Scene::UnregisterDirectionalLight(DirectionalLight* light) { m_CachedDirLight = nullptr; }

    void Scene::UnregisterPointLight(PointLight* light) {
        m_CachedPointLights.erase(std::remove(m_CachedPointLights.begin(), m_CachedPointLights.end(), light),
                                  m_CachedPointLights.end());
    }

    void Scene::UnregisterSpotLight(SpotLight* light) {
        m_CachedSpotLights.erase(std::remove(m_CachedSpotLights.begin(), m_CachedSpotLights.end(), light),
                                 m_CachedSpotLights.end());
    }

} // namespace NFSEngine