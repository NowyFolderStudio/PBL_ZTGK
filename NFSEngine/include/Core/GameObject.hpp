#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Debug/Profiler.hpp"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {

    class Scene;

    class GameObject {
    public:
        std::string name;

        GameObject(Scene* scene, std::string name = "GameObject")
            : m_Scene(scene)
            , name(std::move(name)) {
            auto transform = std::make_unique<Transform>(this);
            m_Transform = transform.get();
            m_Components.push_back(std::move(transform));
        }

        GameObject(std::string name = "GameObject")
            : name(std::move(name)) {
            auto transform = std::make_unique<Transform>(this);
            m_Transform = transform.get();
            m_Components.push_back(std::move(transform));
        }

        void Awake();
        void Start();
        void FixedUpdate(DeltaTime fixedDeltaTime);
        void Update(DeltaTime deltaTime);
        void Render();

        void Destroy() { m_Destroyed = true; }
        bool IsDestroyed() const { return m_Destroyed; }

        bool IsActive() const { return m_Active; }

        void SetActive(bool isActive) { m_Active = isActive; }

        Transform* GetTransform() { return m_Transform; }

        Scene* GetScene() { return m_Scene; }

        const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_Components; }

        template <typename T, typename... Args> T& AddComponent(Args&&... args) {
            T* component = GetComponent<T>();
            if (component) {
                NFS_CORE_WARN("GameObject '{0}' already has component of type: {1}", name, typeid(T).name());
                return *component;
            }
            T* newComponent = new T(this, std::forward<Args>(args)...);

            m_Components.push_back(std::unique_ptr<Component>(newComponent));
            m_Awakened = false;
            m_Started = false;
            return *newComponent;
        }

        template <typename T> T* GetComponent() {
            NFS_PROFILE_FUNCTION();
            for (auto& component : m_Components) {
                if (T* castedComponent = dynamic_cast<T*>(component.get())) {
                    return castedComponent;
                }
            }
            return nullptr;
        }

        template <typename T> void RemoveComponent() {
            static_assert(!std::is_same_v<T, Transform>, "Can't remove Transform component!");

            for (auto it = m_Components.begin(); it != m_Components.end(); ++it) {
                T* casted = dynamic_cast<T*>(it->get());
                if (casted) {
                    if (it->get() == static_cast<Component*>(m_Transform)) {
                        NFS_CORE_ERROR("GameObject '{0}': Can't remove Transform component!", name);
                        return;
                    }

                    m_Components.erase(it);
                    return;
                }
            }

            NFS_CORE_WARN("GameObject '{0}': tried to remove component that not exist", name);
        }

    private:
        std::vector<std::unique_ptr<Component>> m_Components;
        Transform* m_Transform = nullptr;
        Scene* m_Scene = nullptr;

        bool m_Destroyed = false;
        bool m_Active = true;

        bool m_Awakened = false;
        bool m_Started = false;
    };

} // namespace NFSEngine