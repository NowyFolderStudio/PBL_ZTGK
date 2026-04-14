#pragma once

#include "Components/Component.hpp"
#include <glm/glm.hpp>

namespace NFSEngine
{
    class Transform;

    class Camera : public Component
    {
    public:
        Camera(GameObject* owner);

        std::string GetName() const override { return "Camera"; }

        void SetPerspective(float fovDegrees, float nearClip, float farClip);
        void SetViewportSize(uint32_t width, uint32_t height);

        glm::mat4 GetViewMatrix() const;
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

        float GetFOV() const { return m_FOV; }
        void SetFOV(float fov);

    protected:
        virtual void OnAwake() override;

    private:
        void RecalculateProjection();

        Transform* p_Transform = nullptr;
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

        float m_FOV = 45.0f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;
        float m_AspectRatio = 1.778f;
    };
}