#include "Components/ParticleComponent.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Particle.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/MathUtils.hpp"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <memory>

namespace NFSEngine {

    ParticleComponent::ParticleComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material,
                                         const ParticleProperties& properties, float timeOfLife, float particlesPerSecond,
                                         int maxParticles)
        : Component(owner)
        , m_Shader(std::move(shader))
        , m_Material(std::move(material))
        , m_TimeOfLife(timeOfLife)
        , m_ParticlesPerSecond(particlesPerSecond)
        , m_Properties(properties) {

        m_ParticlePool.resize(maxParticles);
        m_InstanceDataBuffer.reserve(maxParticles);

        m_VAO = std::shared_ptr<VertexArray>(VertexArray::Create());

        float quadVertices[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
                                 0.5f,  0.5f,  0.0f, 1.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f };

        auto quadVBO = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
        quadVBO->SetLayout({ { ShaderDataType::Float3, "a_Position" }, { ShaderDataType::Float2, "a_TexCoord" } });
        m_VAO->AddVertexBuffer(quadVBO);

        uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
        auto quadIBO = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t)));
        m_VAO->SetIndexBuffer(quadIBO);

        m_InstanceVBO = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(maxParticles * sizeof(ParticleInstanceData)));
        m_InstanceVBO->SetLayout({ { ShaderDataType::Float3, "a_InstancePosition" },
                                   { ShaderDataType::Float4, "a_InstanceColor" },
                                   { ShaderDataType::Float, "a_InstanceSize" },
                                   { ShaderDataType::Float, "a_InstanceRotation" } });

        m_VAO->AddInstancedVertexBuffer(m_InstanceVBO);
    }

    void ParticleComponent::OnAwake() { m_Transform = m_Owner->GetComponent<Transform>(); }

    void ParticleComponent::Emit(const ParticleProperties& particleProps) {
        Particle& particle = m_ParticlePool[m_PoolIndex];
        particle.active = true;

        if (!m_Transform) m_Transform = m_Owner->GetComponent<Transform>();
        if (!m_Transform) return;
        particle.position = m_Transform->GetWorldPosition() + particleProps.position;

        glm::vec3 velocityVariation;
        velocityVariation.x = particleProps.velocityVariation.x * Math::RandomFloat(-0.5, 0.5);
        velocityVariation.y = particleProps.velocityVariation.y * Math::RandomFloat(-0.5, 0.5);
        velocityVariation.z = particleProps.velocityVariation.z * Math::RandomFloat(-0.5, 0.5);

        particle.velocity = particleProps.velocity + velocityVariation;

        particle.colorBegin = particleProps.colorBegin;
        particle.colorEnd = particleProps.colorEnd;

        particle.lifeTime = particleProps.lifeTime;
        particle.lifeRemaining = particleProps.lifeTime;

        particle.sizeBegin = particleProps.sizeBegin + (particleProps.sizeVariation * (Math::RandomFloat(-0.5, 0.5)));
        particle.sizeEnd = particleProps.sizeEnd;

        particle.rotation = particleProps.rotation + (particleProps.rotationVariation * Math::RandomFloat(-0.5f, 0.5f));
        particle.rotationSpeed
            = particleProps.rotationSpeed + (particleProps.rotationSpeedVariation * Math::RandomFloat(-0.5f, 0.5f));

        m_PoolIndex = (m_PoolIndex + 1) % m_ParticlePool.size();
    }

    void ParticleComponent::EmitMultiple(const ParticleProperties& particleProps, int number) {
        for (int i = 0; i < number; i++) {
            Emit(particleProps);
        }
    }

    void ParticleComponent::OnUpdate(DeltaTime deltaTime) {
        if (m_TimeOfLife > 0) {
            m_ParticleToDraw += m_ParticlesPerSecond * deltaTime;
            int drawParticleCount = (int)m_ParticleToDraw;
            EmitMultiple(m_Properties, drawParticleCount);
            m_ParticleToDraw = std::fmod(m_ParticleToDraw, 1);
        }

        for (auto& particle : m_ParticlePool) {
            if (!particle.active) continue;

            if (particle.lifeRemaining <= 0.0f) {
                particle.active = false;
                continue;
            }

            particle.lifeRemaining -= deltaTime;
            particle.position += particle.velocity * static_cast<float>(deltaTime);
            particle.rotation += particle.rotationSpeed * static_cast<float>(deltaTime);
        }

        m_TimeOfLife -= deltaTime;
        if (m_TimeOfLife + m_Properties.lifeTime < 0) {
            m_Owner->Destroy();
        }
    }

    void ParticleComponent::OnRender() {
        if (!m_Material || !m_Shader) return;

        m_InstanceDataBuffer.clear();

        for (auto& particle : m_ParticlePool) {
            if (!particle.active) continue;

            float lifePct = 1.0f - (particle.lifeRemaining / particle.lifeTime);

            ParticleInstanceData data;
            data.position = particle.position;
            data.size = glm::mix(particle.sizeBegin, particle.sizeEnd, lifePct);
            data.color = glm::mix(particle.colorBegin, particle.colorEnd, lifePct);
            data.rotation = particle.rotation;

            m_InstanceDataBuffer.push_back(data);
        }

        if (m_InstanceDataBuffer.empty()) return;

        uint32_t dataSize = (uint32_t)(m_InstanceDataBuffer.size() * sizeof(ParticleInstanceData));
        m_InstanceVBO->SetData(m_InstanceDataBuffer.data(), dataSize);

        Renderer::SubmitInstanced(m_Shader, m_VAO, m_Material, (uint32_t)m_InstanceDataBuffer.size());
    }
} // namespace NFSEngine