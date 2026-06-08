#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

namespace NFSEngine {
    struct ParticleProperties {
        glm::vec3 position = { 0, 0, 0 };
        glm::vec3 velocity = { 0, 0, 0 };
        glm::vec3 velocityVariation = { 0, 0, 0 };
        glm::vec4 colorBegin = { 1, 1, 1, 1 };
        glm::vec4 colorEnd = { 1, 1, 1, 1 };
        float sizeBegin = 1;
        float sizeEnd = 1;
        float sizeVariation = 0;
        float lifeTime = 1;
        float rotation = 0.0f;
        float rotationVariation = 0.0f;
        float rotationSpeed = 0.0f;
        float rotationSpeedVariation = 0.0f;
    };

    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec4 colorBegin, colorEnd;

        float rotation = 0.0f;
        float rotationSpeed = 0.0f;
        float sizeBegin, sizeEnd;

        float lifeTime = 1.0f;
        float lifeRemaining = 0.0f;

        bool active = false;
    };
} // namespace NFSEngine