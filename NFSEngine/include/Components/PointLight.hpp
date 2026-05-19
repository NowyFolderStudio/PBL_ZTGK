#pragma once

#include <glm/glm.hpp>

#include "Core/GameObject.hpp"

#include "Components/Component.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine {
    class PointLight : public Component {
    public:
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;

        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;

        PointLight(GameObject* owner)
            : Component(owner) { }

        ~PointLight();

        virtual std::string GetName() const override { return "PointLight"; }

        virtual void OnAwake() override;

        Transform* GetTransform() const { return m_Transform; }

        void OnImGuiRender() override;

    private:
        Transform* m_Transform = nullptr;
    };
} // namespace NFSEngine