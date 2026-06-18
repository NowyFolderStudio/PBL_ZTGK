#pragma once
#include <NFSEngine.h>
#include <functional>
#include "Renderer/Material.hpp"
#include "Renderer/Particle.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

class DestructibleComponent : public NFSEngine::Component {
public:
    DestructibleComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "DestructibleComponent"; }

    int MaxHealth = 1;
    bool SpawnParticlesOnDeath = true;

    std::function<void()> OnDestroyed = nullptr;

    void TakeDamage(int amount, glm::vec3 hitSourcePos = glm::vec3(0.0f)) {
        if (m_IsDead) return;

        m_CurrentHealth -= amount;

        if (m_CurrentHealth <= 0) {
            Die();
        } else {
            ApplyKnockback(hitSourcePos);
        }
    }

    bool IsDead() const { return m_IsDead; }

protected:
    int m_CurrentHealth = 1;
    bool m_IsDead = false;

    std::shared_ptr<NFSEngine::Material> m_DestroyMaterial;
    std::shared_ptr<NFSEngine::Shader> m_ParticleShader;

    void OnStart() override {
        m_CurrentHealth = MaxHealth;

        if (SpawnParticlesOnDeath) {
            auto texture = NFSEngine::Texture::Create("assets/textures/particles/star.png");
            m_DestroyMaterial = std::make_shared<NFSEngine::Material>();
            m_DestroyMaterial->AlbedoMap = texture;
            m_ParticleShader
                = NFSEngine::Shader::Create("StarParticle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
        }
    }

    void Die() {
        m_IsDead = true;

        if (OnDestroyed) {
            OnDestroyed();
        }

        if (SpawnParticlesOnDeath && m_DestroyMaterial && m_ParticleShader) {
            SpawnDeathParticles();
        }

        GetOwner()->SetActive(false);
    }

    void SpawnDeathParticles() {
        NFSEngine::ParticleProperties p;
        p.lifeTime = 0.5f;
        p.colorBegin = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        p.colorEnd = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        p.sizeBegin = 1.0f;
        p.sizeEnd = 0.1f;
        p.sizeVariation = 0.5f;
        p.rotationVariation = 360.0f;
        p.velocityVariation = glm::vec3(15.0f, 15.0f, 15.0f);

        glm::vec3 spawnPos = GetOwner()->GetTransform()->GetWorldPosition();
        NFSEngine::ParticleFactory::Create(GetOwner()->GetScene(), m_DestroyMaterial, m_ParticleShader, p, 0.1f, 100, 30,
                                           spawnPos);
    }

    void ApplyKnockback(glm::vec3 hitSourcePos) {
        auto* rb = GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (rb && glm::length(hitSourcePos) > 0.01f) {
            glm::vec3 myPos = GetOwner()->GetTransform()->GetWorldPosition();
            glm::vec3 pushDir = glm::normalize(myPos - hitSourcePos);
            pushDir.y = 0.5f;
            rb->Velocity += pushDir * 15.0f;
        }
    }
};