#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Components/ParticleComponent.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Particle.hpp"
#include <memory>

namespace NFSEngine {
    class ParticleFactory {
    public:
        // clang-format off
            static void Create(Scene* scene,
                const std::shared_ptr<Material>& material,
                const std::shared_ptr<Shader>& shader,
                const ParticleProperties& properties,
                float timeOfLife = 0.1f,
                float particlesPerSecond = 60,
                int maxParticles = 100,
                glm::vec3 position = { 0, 0, 0 },
                Transform* parentTransform = nullptr) {
                    auto* go = scene->CreateGameObject("ParticleObject");
                    go->AddComponent<ParticleComponent>(shader, material, properties, timeOfLife, particlesPerSecond, maxParticles);
                    if (parentTransform) {
                        go->GetTransform()->SetParent(parentTransform);
                    }
                    go->GetTransform()->SetPosition(position);
                }
        // clang-format on
    };
} // namespace NFSEngine