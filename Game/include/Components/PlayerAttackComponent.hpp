#pragma once
#include <NFSEngine.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "Components/Aura/AuraManager.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Particle.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

class PlayerAttackComponent : public NFSEngine::Component {
public:
    PlayerAttackComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "PlayerAttackComponent"; }

    float AttackRadius = 5.0f;

protected:
    NFSEngine::SphereCollider3DComponent* m_Sphere = nullptr;
    std::vector<NFSEngine::GameObject*> m_EnemiesInRange;

    size_t m_AuraEventId = 0;

    std::shared_ptr<NFSEngine::Material> m_StarMaterial;
    std::shared_ptr<NFSEngine::Shader> m_ParticleShader;

    void OnAwake() override { }

    void OnStart() override {
        auto texture = NFSEngine::Texture::Create("assets/textures/particles/star.png");
        m_StarMaterial = std::make_shared<NFSEngine::Material>();
        m_StarMaterial->AlbedoMap = texture;
        m_ParticleShader
            = NFSEngine::Shader::Create("StarParticle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");

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

        if (AuraManager::Instance) {
            m_AuraEventId = AuraManager::Instance->OnAuraChanged.AddListener([this](AuraType newAura) { this->PerformAttack(); });
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        m_EnemiesInRange.erase(std::remove_if(m_EnemiesInRange.begin(), m_EnemiesInRange.end(),
                                              [](NFSEngine::GameObject* go) { return !go->IsActive(); }),
                               m_EnemiesInRange.end());
    }

    void OnEnable() override { }

    void OnDisable() override {
        if (AuraManager::Instance && m_AuraEventId != 0) {
            AuraManager::Instance->OnAuraChanged.RemoveListener(m_AuraEventId);
            m_AuraEventId = 0;
        }
    }

private:
    void PerformAttack() {
        NFSEngine::ParticleProperties properties;
        properties.lifeTime = 0.3f;
        properties.colorBegin = glm::vec4(0.949f, 0.851f, 0.207f, 1.0f);
        properties.colorEnd = glm::vec4(0.949f, 0.851f, 0.207f, 0.0f);

        properties.sizeBegin = 1.8f;
        properties.sizeEnd = 0.2f;
        properties.sizeVariation = 0.5f;
        properties.rotationVariation = 360.0f;

        properties.velocityVariation = glm::vec3(25.0f, 25.0f, 25.0f);

        glm::vec3 spawnPosition = m_Owner->GetTransform()->GetWorldPosition() + glm::vec3(0.0f, 1.0f, 0.0f);

        NFSEngine::ParticleFactory::Create(m_Owner->GetScene(), m_StarMaterial, m_ParticleShader, properties, 0.1f, 1000, 250,
                                           spawnPosition);

        if (m_EnemiesInRange.empty()) return;

        for (auto* enemy : m_EnemiesInRange) {
            enemy->SetActive(false); // TODO: Replace with proper damage handling and death logic
        }

        m_EnemiesInRange.clear();
    }
};