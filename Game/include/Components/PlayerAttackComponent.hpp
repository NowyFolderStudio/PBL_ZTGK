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
#include "Components/DestructibleComponent.hpp"
#include "Components/AttackInteractableComponent.hpp"

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

    std::shared_ptr<NFSEngine::Material> m_GuitarMaterial;
    std::shared_ptr<NFSEngine::Material> m_PianoMaterial;
    std::shared_ptr<NFSEngine::Shader> m_ParticleShader;

    void OnAwake() override { }

    void OnStart() override {
        auto texture = NFSEngine::Texture::Create("assets/textures/particles/guitar_particle.png");
        m_GuitarMaterial = std::make_shared<NFSEngine::Material>();
        m_GuitarMaterial->AlbedoMap = texture;
        texture = NFSEngine::Texture::Create("assets/textures/particles/piano_particle.png");
        m_PianoMaterial = std::make_shared<NFSEngine::Material>();
        m_PianoMaterial->AlbedoMap = texture;
        m_ParticleShader
            = NFSEngine::Shader::Create("StarParticle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");

        m_Sphere = &GetOwner()->AddComponent<NFSEngine::SphereCollider3DComponent>();
        if (m_Sphere) {
            m_Sphere->IsTrigger = true;
            m_Sphere->Radius = AttackRadius;

            m_Sphere->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (other->GetComponent<DestructibleComponent>() != nullptr ||
                    other->GetComponent<AttackInteractableComponent>() != nullptr) {
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
        properties.colorBegin = glm::vec4(1, 1, 1, 0.8f);
        properties.colorEnd = glm::vec4(1, 1, 1, 0.0f);

        properties.sizeBegin = 1.8f;
        properties.sizeEnd = 0.5f;
        properties.sizeVariation = 0.5f;
        properties.rotationVariation = 360.0f;

        properties.velocityVariation = glm::vec3(20.0f, 20.0f, 20.0f);

        glm::vec3 spawnPosition = m_Owner->GetTransform()->GetWorldPosition() + glm::vec3(0.0f, 1.0f, 0.0f);

        if (AuraManager::Instance->CurrentAura == AuraType::Second) {
            NFSEngine::ParticleFactory::Create(m_Owner->GetScene(), m_GuitarMaterial, m_ParticleShader, properties, 0.1f, 1000,
                                               250, spawnPosition);
        }
        if (AuraManager::Instance->CurrentAura == AuraType::First) {
            NFSEngine::ParticleFactory::Create(m_Owner->GetScene(), m_PianoMaterial, m_ParticleShader, properties, 0.1f, 1000,
                                               250, spawnPosition);
        }

        if (m_EnemiesInRange.empty()) return;

        glm::vec3 myPos = m_Owner->GetTransform()->GetWorldPosition();

        for (auto* target : m_EnemiesInRange) {
            // Szukamy Destructible. Jeśli to wróg - zada HP. Jeśli to beczka - zada HP.
            // Jeśli to np. ściana bez DestructibleComponent - zignoruje ją!
            if (auto* destComp = target->GetComponent<DestructibleComponent>()) {
                destComp->TakeDamage(1, myPos);
            }

            if (auto* interactable = target->GetComponent<AttackInteractableComponent>()) {
                if (interactable->OnHit) {
                    interactable->OnHit();
                }
            }
        }
    }
};