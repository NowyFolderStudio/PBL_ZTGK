#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include <glm/ext.hpp>

namespace NFSEngine {
    Camera::Camera(GameObject* owner)
        : Component(owner) {
        RecalculateProjection();
    }

    void Camera::OnAwake() { p_Transform = m_Owner->GetComponent<Transform>(); }

    void Camera::SetPerspective(float fovDegrees, float nearClip, float farClip) {
        m_FOV = fovDegrees;
        m_NearClip = nearClip;
        m_FarClip = farClip;
        RecalculateProjection();
    }

    void Camera::SetViewportSize(uint32_t width, uint32_t height) {
        if (height == 0) return;
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjection();
    }

    void Camera::SetFOV(float fov) {
        m_FOV = fov;
        RecalculateProjection();
    }

    glm::mat4 Camera::GetViewMatrix() const {
        if (p_Transform) {
            return glm::inverse(p_Transform->GetGlobalMatrix());
        }
        return { 1.0f };
    }

    void Camera::RecalculateProjection() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }
} // namespace NFSEngine
