#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components/Component.hpp"

namespace NFSEngine {
    class DirectionalLight : public Component {
    public:
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
        float Intensity = 1.0f;

        DirectionalLight(GameObject* owner)
            : Component(owner) { }

        ~DirectionalLight();

        virtual std::string GetName() const override { return "DirectionalLight"; }

        void OnAwake() override;

        void OnImGuiRender() override;
    };
} // namespace NFSEngine