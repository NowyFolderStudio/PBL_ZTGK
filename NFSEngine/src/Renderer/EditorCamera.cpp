#include "Renderer/EditorCamera.hpp"
#include "Core/Input.hpp"
#include "Core/KeyCodes.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace NFSEngine {

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip) {
        UpdateProjection();
        UpdateView();
    }

    void EditorCamera::UpdateProjection() {
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView() {
        glm::quat orientation = GetOrientation();
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    void EditorCamera::OnUpdate(DeltaTime dt) {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
        m_InitialMousePosition = mouse;

        if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
            m_Yaw   -= delta.x * RotationSpeed();
            m_Pitch -= delta.y * RotationSpeed();

            float moveSpeed = 10.0f * dt;
            if (Input::IsKeyPressed(Key::LeftShift)) moveSpeed *= 2.5f;

            if (Input::IsKeyPressed(Key::W)) m_Position += GetForwardDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::S)) m_Position -= GetForwardDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::A)) m_Position -= GetRightDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::D)) m_Position += GetRightDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::E)) m_Position += glm::vec3(0, 1, 0) * moveSpeed;
            if (Input::IsKeyPressed(Key::Q)) m_Position -= glm::vec3(0, 1, 0) * moveSpeed;

            m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;
        }
        else if (Input::IsKeyPressed(Key::LeftAlt) && Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
            MouseRotate(delta);
            m_Position = CalculatePosition();
        }
        else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle)) {
            MousePan(delta);
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) { return this->OnMouseScroll(event); });
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
        float delta = e.GetYOffset();
        m_Position += GetForwardDirection() * (delta * 2.0f);
        m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta) {
        auto [xSpeed, ySpeed] = std::pair<float, float>{ PanSpeed(), PanSpeed() };
        glm::vec3 offset = -GetRightDirection() * delta.x * xSpeed * m_Distance + GetUpDirection() * delta.y * ySpeed * m_Distance;

        m_FocalPoint += offset;
        m_Position += offset;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta) {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw -= yawSign * delta.x * RotationSpeed();
        m_Pitch -= delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta) {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f) {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::CalculatePosition() const {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    glm::quat EditorCamera::GetOrientation() const {
        return glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
    }

    glm::vec3 EditorCamera::GetUpDirection() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
    glm::vec3 EditorCamera::GetRightDirection() const { return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
    glm::vec3 EditorCamera::GetForwardDirection() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)); }

    float EditorCamera::RotationSpeed() const { return 0.8f; }
    float EditorCamera::PanSpeed() const { return 0.03f; }
    float EditorCamera::ZoomSpeed() const { return m_Distance * 0.2f; }
}