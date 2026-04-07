#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Core/Components/Component.hpp"
#include "Core/DeltaTime.hpp"

namespace NFSEngine {

    class GameObject {
        public:
            std::string Name;

            GameObject(const std::string& name) : Name(name) {}

            void Awake();
            void Start();
            void Update(DeltaTime deltaTime);
            void Reder();

            template<typename T, typename... Args>
            T& AddComponent(Args&&... args) {
                T* newComponent = new T(this, std::forward<Args>(args)...);
                
                m_Components.push_back(std::unique_ptr<Component>(newComponent));
                return *newComponent;
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
    };

}