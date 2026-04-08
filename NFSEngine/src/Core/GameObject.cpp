#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {
    void GameObject::Awake() {
        for (const auto& component : m_Components) {
            component->Awake();
        }
    }

    void GameObject::Start() {
        for (const auto& component : m_Components) {
            component->Start();
        }
    }

    void GameObject::Update(DeltaTime deltaTime) {
        if (m_Active == false) return;

        for (const auto& component : m_Components) {
            component->Update(deltaTime);
        }
    }

    void GameObject::Render() {
        if (m_Active == false) return;
        
        for (const auto& component : m_Components) {
            component->Render();
        }
    }
}