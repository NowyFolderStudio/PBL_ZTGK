#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {
    void GameObject::Awake() {
        if (m_Awakened) return;
        for (size_t i = 0; i < m_Components.size(); i++) {
            auto& component = m_Components[i];
            component->Awake();
        }
        m_Awakened = true;
    }

    void GameObject::Start() {
        if (m_Started) return;
        for (size_t i = 0; i < m_Components.size(); i++) {
            auto& component = m_Components[i];
            component->Start();
        }
        m_Started = true;
    }

    void GameObject::FixedUpdate(DeltaTime fixedDeltaTime) {
        if (!m_Active || m_Destroyed) return;

        for (size_t i = 0; i < m_Components.size(); i++) {
            auto& component = m_Components[i];
            component->FixedUpdate(fixedDeltaTime);
        }
    }

    void GameObject::Update(DeltaTime deltaTime) {
        if (!m_Active || m_Destroyed) return;

        for (size_t i = 0; i < m_Components.size(); i++) {
            auto& component = m_Components[i];
            component->Update(deltaTime);
        }
    }

    void GameObject::Render() {
        if (!m_Active || m_Destroyed) return;

        for (size_t i = 0; i < m_Components.size(); i++) {
            auto& gameObject = m_Components[i];
            gameObject->Render();
        }
    }
} // namespace NFSEngine