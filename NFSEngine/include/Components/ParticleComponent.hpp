#pragma once

#include "Core/DeltaTime.hpp"
#include "nfspch.h"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"

#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Particle.hpp"
#include "Renderer/VertexArray.hpp"

namespace NFSEngine {

    class ParticleComponent : public Component {
    public:
        ParticleComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material,
                          const ParticleProperties& properties, float timeOfLife = 0.1f, float particlesPerSecond = 60,
                          int maxParticles = 100);

        std::string GetName() const override { return "ParticleComponent"; }

        void Emit(const ParticleProperties& particleProps);
        void EmitMultiple(const ParticleProperties& particleProps, int number);

    protected:
        void OnAwake() override;
        void OnUpdate(DeltaTime deltaTime) override;
        void OnRender() override;

    private:
        std::vector<Particle> m_ParticlePool;
        size_t m_PoolIndex = 0;
        ParticleProperties m_Properties;

        std::shared_ptr<Material> m_Material;
        std::shared_ptr<Shader> m_Shader;
        Transform* m_Transform = nullptr;

        float m_TimeOfLife;
        float m_ParticlesPerSecond;

        float m_ParticleToDraw = 0.0f;

        struct ParticleInstanceData {
            glm::vec3 position;
            glm::vec4 color;
            float size;
            float rotation;
        };
        std::vector<ParticleInstanceData> m_InstanceDataBuffer;
        std::shared_ptr<VertexArray> m_VAO;
        std::shared_ptr<VertexBuffer> m_InstanceVBO;
    };
} // namespace NFSEngine