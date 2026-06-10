#pragma once

#include <NFSEngine.h>
#include <cmath>
#include <glm/ext/vector_float3.hpp>

#include "Components/Managers/ScoreManager.hpp"
#include "Components/Transform.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Particle.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

class CoinComponent : public NFSEngine::Component {
public:
    explicit CoinComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "CoinComponent"; }

    int ScoreValue = 67;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        m_ModelTransform = m_Owner->GetTransform()->GetChild(0);
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Collected) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            m_Collected = true;

            auto texture = NFSEngine::Texture::Create("assets/textures/particles/note_particle.png");

            auto material = std::make_shared<NFSEngine::Material>();
            material->AlbedoMap = texture;

            auto shader
                = NFSEngine::Shader::Create("NoteParticle", "assets/shaders/particle.vert", "assets/shaders/particle.frag");

            NFSEngine::ParticleProperties properties;
            properties.lifeTime = 0.3;
            properties.colorBegin = glm::vec4(0.949, 0.851, 0.207, 1.0);
            properties.colorEnd = glm::vec4(0.949, 0.851, 0.207, 5.0);
            properties.sizeBegin = 0.7;
            properties.sizeEnd = 0.0;
            properties.sizeVariation = 0.2;
            properties.rotationVariation = 90;
            properties.velocityVariation = glm::vec3(6, 6, 6);

            NFSEngine::ParticleFactory::Create(m_Owner->GetScene(), material, shader, properties, 0.1, 60, 100,
                                               GetOwner()->GetTransform()->GetWorldPosition());

            if (ScoreManager::Instance) {
                ScoreManager::Instance->AddScore(ScoreValue);
            }

            m_Owner->Destroy();
        };
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        m_AnimationTime += deltaTime;
        m_ModelTransform->Rotate(glm::vec3(0, 0, 1) * static_cast<float>(deltaTime) * m_RotationSpeed);
        m_ModelTransform->SetPosition(glm::vec3(0, std::sin(m_AnimationTime * m_FloatSpeed) * m_FloatAmplitude, 0));
    }

private:
    bool m_Collected = false;
    NFSEngine::Transform* m_ModelTransform;
    float m_AnimationTime = 0;
    float m_RotationSpeed = 45.0;
    float m_FloatSpeed = 3.0;
    float m_FloatAmplitude = 0.3;
};
