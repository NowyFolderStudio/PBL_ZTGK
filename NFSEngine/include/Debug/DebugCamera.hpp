#pragma once
#include "Core/DeltaTime.hpp"
#include "Core/Input.hpp"
#include "Events/Event.hpp"
#include "Events/MouseEvent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NFSEngine {

    class DebugCamera {
    public:
        static void Init(float fov, float aspectRatio, float nearClip, float farClip);
        static DebugCamera& Get();

        static bool IsActive() { return s_Active; }
        static void SetActive(bool active);

        static void OnUpdate(DeltaTime dt);
        static void OnEvent(Event& e);

        static const glm::mat4& GetViewMatrix() { return Get().m_ViewMatrix; }
        static const glm::mat4& GetProjection() { return Get().m_Projection; }
        static glm::mat4 GetViewProjection() { return Get().m_Projection * Get().m_ViewMatrix; }
        static const glm::vec3& GetPosition() { return Get().m_Position; }
        static glm::quat GetOrientation() { return Get().GetOrientationImpl(); }
        static void ResetMousePosition() { Get().m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() }; }
        static void SyncFromTransform(const glm::vec3& position, const glm::quat& rotation) {
            Get().SetViewFromPosition(position, rotation);
        }

    private:
        DebugCamera() = default;
        DebugCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void SetViewFromPosition(const glm::vec3& position, const glm::quat& orientation);

        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);
        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;
        glm::quat GetOrientationImpl() const;

        float PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;

        glm::vec3 GetForwardDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetUpDirection() const;

    private:
        static bool s_Active;

        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        glm::mat4 m_ViewMatrix{};
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
        bool m_RMBHeld = false;

        glm::mat4 m_Projection{};
    };
}
