#pragma once

#include <NFSEngine.h>

#include <vector>
#include <algorithm>

class PlayerAttackComponent : public NFSEngine::Component {
public:
    PlayerAttackComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "PlayerAttackComponent"; }

    float AttackRadius = 5.0f;

protected:
    NFSEngine::SphereCollider3DComponent* m_Sphere = nullptr;
    std::vector<NFSEngine::GameObject*> m_EnemiesInRange;

    void OnAwake() override { }

    void OnStart() override {
        m_Sphere = &GetOwner()->AddComponent<NFSEngine::SphereCollider3DComponent>();
        if (m_Sphere) {
            m_Sphere->IsTrigger = true;
            m_Sphere->Radius = AttackRadius;

            m_Sphere->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (other->CompareTag(NFSEngine::Tags::Enemy)) {
                    m_EnemiesInRange.push_back(other);
                }
            };

            m_Sphere->OnTriggerExit = [this](NFSEngine::GameObject* other) {
                auto it = std::find(m_EnemiesInRange.begin(), m_EnemiesInRange.end(), other);
                if (it != m_EnemiesInRange.end()) {
                    m_EnemiesInRange.erase(it);
                }
            };
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        m_EnemiesInRange.erase(std::remove_if(m_EnemiesInRange.begin(), m_EnemiesInRange.end(),
                                              [](NFSEngine::GameObject* go) { return !go->IsActive(); }),
                               m_EnemiesInRange.end());

        if (NFSEngine::InputActionManager::IsDown("Attack")) {
            PerformAttack();
        }
    }

    void OnEnable() override { }

    void OnDisable() override { }

private:
    void PerformAttack() {
        if (m_EnemiesInRange.empty()) return;
        for (auto* enemy : m_EnemiesInRange) {
            enemy->SetActive(false); // TODO: Replace with proper damage handling and death logic
        }

        m_EnemiesInRange.clear();
    }
};
