#include "Core/Scene.hpp"
#include <algorithm>

namespace NFSEngine {

    GameObject* Scene::CreateGameObject(const std::string& name) {
        auto go = std::make_unique<GameObject>(this, name);
        GameObject* rawPtr = go.get();
        m_GameObjects.push_back(std::move(go));
        return rawPtr;
    }

    void Scene::OnUpdate(DeltaTime deltaTime) {

        for (size_t i = 0; i < m_GameObjects.size(); i++) {
            auto& gameObject = m_GameObjects[i];
            gameObject->Awake();
            gameObject->Start();
        }

        if (deltaTime > 0.25f) deltaTime = 0.25f;
        m_Accumulator += deltaTime;
        
        while (m_Accumulator >= m_FixedDeltaTime) {
            for (size_t i = 0; i < m_GameObjects.size(); i++) {
                auto& gameObject = m_GameObjects[i];
                gameObject->FixedUpdate(m_FixedDeltaTime);
            }
            m_Accumulator -= m_FixedDeltaTime;
        }

        for (size_t i = 0; i < m_GameObjects.size(); i++) {
            auto& gameObject = m_GameObjects[i];
            gameObject->Update(deltaTime);
        }

        m_GameObjects.erase(
            std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
                [](const std::unique_ptr<GameObject>& gameObject) {
                    return gameObject->IsDestroyed();
                }),
            m_GameObjects.end()
        );
    }

    void Scene::OnRender() {
        for (auto& gameObject : m_GameObjects) {
            gameObject->Render();
        }
    }

}