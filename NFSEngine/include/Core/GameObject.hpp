#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "Core/Components/Component.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {

    class Scene;

    class GameObject {
        public:
            std::string Name;

            GameObject(Scene* scene, const std::string& name = "GameObject") : p_Scene(scene), Name(name) {}

            GameObject(const std::string& name = "GameObject") : p_Scene(nullptr), Name(name) {}

            void Awake();
            void Start();
            void FixedUpdate(DeltaTime fixedDeltaTime);
            void Update(DeltaTime deltaTime);
            void Render();

            void Destroy() { m_Destroyed = true; }
            bool IsDestroyed() { return m_Destroyed; }

            bool IsActive() { return m_Active; }
            
            void SetActive(bool isActive) { m_Active = isActive; }

            Scene* GetScene() { return p_Scene; }

            template<typename T, typename... Args>
            T& AddComponent(Args&&... args) {
                T* newComponent = new T(this, std::forward<Args>(args)...);
                
                m_Components.push_back(std::unique_ptr<Component>(newComponent));
                return *newComponent;
                m_Awakened = false;
                m_Started = false;
            }

            template<typename T>
            T* GetComponent() {
                for (auto& component : m_Components) {
                    if (T* castedComponent = dynamic_cast<T*>(component.get())) {
                        return castedComponent;
                    }
                }
                return nullptr;
            }

        private:
            std::vector<std::unique_ptr<Component>> m_Components;
            Scene* p_Scene = nullptr;

            bool m_Destroyed = false;
            bool m_Active = true;

            bool m_Awakened = false;
            bool m_Started = false;
    };

}