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

    class ParticleEmitterComponent : public Component {
    public:
        ParticleEmitterComponent(GameObject* owner, size_t maxParticles, std::shared_ptr<Shader> shader,
                                 std::shared_ptr<Material> material);

        std::string GetName() const override { return "ParticleEmitterComponent"; }

        void Emit(const ParticleProperties& particleProps);
        void EmitMultiple(const ParticleProperties& particleProps, int number);

    protected:
        void OnAwake() override;
        void OnUpdate(DeltaTime deltaTime) override;
        void OnRender() override;

    private:
        std::vector<Particle> m_ParticlePool;
        size_t m_PoolIndex = 0;

        std::shared_ptr<Material> m_Material;
        std::shared_ptr<Shader> m_Shader;
        Transform* m_Transform = nullptr;

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