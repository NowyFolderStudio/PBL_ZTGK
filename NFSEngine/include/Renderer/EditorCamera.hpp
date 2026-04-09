#pragma once
#include "Core/DeltaTime.hpp"
#include "Events/Event.hpp"
#include "Events/MouseEvent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NFSEngine {

    class EditorCamera {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate(DeltaTime dt);
        void OnEvent(Event& e);

        [[nodiscard]] inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }

        [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        [[nodiscard]] glm::mat4 GetProjection() const { return m_Projection; }
        [[nodiscard]] glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

        [[nodiscard]] glm::vec3 GetUpDirection() const;
        [[nodiscard]] glm::vec3 GetRightDirection() const;
        [[nodiscard]] glm::vec3 GetForwardDirection() const;
        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
        
    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        [[nodiscard]] glm::vec3 CalculatePosition() const;
        [[nodiscard]] glm::quat GetOrientation() const;

        [[nodiscard]] float PanSpeed() const;
        [[nodiscard]] float RotationSpeed() const;
        [[nodiscard]] float ZoomSpeed() const;

    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        glm::mat4 m_ViewMatrix{};
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;

        glm::mat4 m_Projection{};
    };
}