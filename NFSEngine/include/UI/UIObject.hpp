#pragma once

#include <vector>
#include <memory>
#include "UIComponents.hpp"

namespace NFSEngine {
    class UIObject {
    private:
        std::vector<std::unique_ptr<UIComponent>> m_Components;

    public:
        RectTransform Transform;

        UIObject() = default;
        ~UIObject() = default;

        template <typename T, typename... Args> T& AddComponent(Args&&... args) {
            auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);

            newComponent->Owner = this;

            T& componentRef = *newComponent;
            m_Components.push_back(std::move(newComponent));

            return componentRef;
        }

        template <typename T> bool HasComponent() const {
            for (const auto& component : m_Components) {
                if (dynamic_cast<T*>(component.get()) != nullptr) {
                    return true;
                }
            }
            return false;
        }

        template <typename T> T* GetComponent() const {
            for (const auto& component : m_Components) {
                if (auto castedComponent = dynamic_cast<T*>(component.get())) {
                    return castedComponent;
                }
            }
            return nullptr;
        }
    };
} // namespace NFSEngine