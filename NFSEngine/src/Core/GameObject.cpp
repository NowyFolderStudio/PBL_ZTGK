#include "Core/GameObject.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {
    void GameObject::Awake() {
        for (const auto& component : m_Components) {
            component->OnAwake();
        }
    }

    void GameObject::Start() {
        for (const auto& component : m_Components) {
            component->OnStart();
        }
    }

    void GameObject::Update(DeltaTime deltaTime) {
        for (const auto& component : m_Components) {
            component->OnUpdate(deltaTime);
        }
    }

    void GameObject::Render() {
        for (const auto& component : m_Components) {
            component->OnRender();
        }
    }
}