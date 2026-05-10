#pragma once

#include "Components/Component.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/GameObject.hpp"

#include <functional>

namespace NFSEngine {

    class CoinComponent : public Component {
    public:
        explicit CoinComponent(GameObject* owner)
            : Component(owner) {}

        std::string GetName() const override { return "CoinComponent"; }

        std::function<void()> OnCollected;

    protected:
        void OnStart() override {
            auto* collider = m_Owner->GetComponent<ColliderComponent>();
            if (collider) {
                collider->IsTrigger = true;
                collider->OnTriggerEnter = [this](GameObject* other) {
                    if (m_Collected) return;
                    if (other->name == "Player") {
                        m_Collected = true;
                        if (OnCollected) OnCollected();
                        m_Owner->SetActive(false);
                    }
                };
            }
        }

    private:
        bool m_Collected = false;
    };

} // namespace NFSEngine
