#include "Debug/DebugCamera.hpp"
#include "Core/Input.hpp"
#include "Core/KeyCodes.hpp"
#include "Core/Application.hpp"

#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
namespace NFSEngine {

    static std::unique_ptr<DebugCamera> s_Instance;
    bool DebugCamera::s_Active = false;

    void DebugCamera::Init(float fov, float aspectRatio, float nearClip, float farClip) {
        if (!s_Instance) {
            s_Instance = std::unique_ptr<DebugCamera>(new DebugCamera(fov, aspectRatio, nearClip, farClip));
        }
    }

    DebugCamera& DebugCamera::Get() {
        if (!s_Instance) {
            s_Instance = std::unique_ptr<DebugCamera>(new DebugCamera());
        }
        return *s_Instance;
    }

    void DebugCamera::SetActive(bool active) {
        bool wasActive = s_Active;
        s_Active = active;

        if (active && !wasActive) {
            Get().m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };
            Application::Get().GetWindow().SetCursorMode(CursorMode::Normal);
        }
    }

    DebugCamera::DebugCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip) {
        UpdateProjection();
        UpdateView();
    }

    void DebugCamera::SetViewFromPosition(const glm::vec3& position, const glm::quat& orientation) {
        m_Position = position;
        // Get forward vector directly from the orientation quaternion
        glm::vec3 forward = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
        // Derive pitch (angle from horizontal) and yaw (angle around Y axis)
        m_Pitch = std::asin(glm::clamp(forward.y, -1.0f, 1.0f));
        m_Yaw = std::atan2(forward.x, forward.z);
        m_FocalPoint = m_Position + forward * m_Distance;
        UpdateView();
    }

    void DebugCamera::UpdateProjection() {
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void DebugCamera::UpdateView() {
        glm::quat orientation = GetOrientationImpl();
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    void DebugCamera::OnUpdate(DeltaTime dt) {
        if (!s_Active || !s_Instance) return;
        auto& cam = *s_Instance;

        // CursorMode::Locked = GLFW_CURSOR_DISABLED, which ImGui skips in UpdateMouseCursor().
        bool rmbHeld = Input::IsMouseButtonPressed(Mouse::ButtonRight);
        if (rmbHeld) {
            if (!cam.m_RMBHeld) {
                // Just pressed: switch to Locked mode and reset mouse tracking
                cam.m_RMBHeld = true;
                Application::Get().GetWindow().SetCursorMode(CursorMode::Locked);
                cam.m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };
            }
        } else {
            if (cam.m_RMBHeld) {
                cam.m_RMBHeld = false;
                Application::Get().GetWindow().SetCursorMode(CursorMode::Normal);
            }
        }

        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - cam.m_InitialMousePosition) * 0.003f;
        cam.m_InitialMousePosition = mouse;

        if (rmbHeld) {
            cam.m_Yaw   -= delta.x * cam.RotationSpeed();
            cam.m_Pitch -= delta.y * cam.RotationSpeed();

            float moveSpeed = 10.0f * dt;
            if (Input::IsKeyPressed(Key::LeftShift)) moveSpeed *= 2.5f;

            if (Input::IsKeyPressed(Key::W)) cam.m_Position += cam.GetForwardDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::S)) cam.m_Position -= cam.GetForwardDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::A)) cam.m_Position -= cam.GetRightDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::D)) cam.m_Position += cam.GetRightDirection() * moveSpeed;
            if (Input::IsKeyPressed(Key::E)) cam.m_Position += glm::vec3(0, 1, 0) * moveSpeed;
            if (Input::IsKeyPressed(Key::Q)) cam.m_Position -= glm::vec3(0, 1, 0) * moveSpeed;

            cam.m_FocalPoint = cam.m_Position + cam.GetForwardDirection() * cam.m_Distance;
        }
        else if (Input::IsKeyPressed(Key::LeftAlt) && Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
            cam.MouseRotate(delta);
            cam.m_Position = cam.CalculatePosition();
        }
        else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle)) {
            cam.MousePan(delta);
        }

        cam.UpdateView();
    }

    void DebugCamera::OnEvent(Event& e) {
        if (!s_Active || !s_Instance) return;
        auto& cam = *s_Instance;

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>([&cam](MouseScrolledEvent& event) { return cam.OnMouseScroll(event); });
    }

    bool DebugCamera::OnMouseScroll(MouseScrolledEvent& e) {
        float delta = e.GetYOffset();
        m_Position += GetForwardDirection() * (delta * 2.0f);
        m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;
        UpdateView();
        return false;
    }

    void DebugCamera::MousePan(const glm::vec2& delta) {
        auto [xSpeed, ySpeed] = std::pair<float, float>{ PanSpeed(), PanSpeed() };
        glm::vec3 offset = -GetRightDirection() * delta.x * xSpeed * m_Distance + GetUpDirection() * delta.y * ySpeed * m_Distance;

        m_FocalPoint += offset;
        m_Position += offset;
    }

    void DebugCamera::MouseRotate(const glm::vec2& delta) {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw -= yawSign * delta.x * RotationSpeed();
        m_Pitch -= delta.y * RotationSpeed();
    }

    void DebugCamera::MouseZoom(float delta) {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f) {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    glm::vec3 DebugCamera::CalculatePosition() const {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    glm::quat DebugCamera::GetOrientationImpl() const {
        return glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
    }

    glm::vec3 DebugCamera::GetUpDirection() const { return glm::rotate(GetOrientationImpl(), glm::vec3(0.0f, 1.0f, 0.0f)); }
    glm::vec3 DebugCamera::GetRightDirection() const { return glm::rotate(GetOrientationImpl(), glm::vec3(1.0f, 0.0f, 0.0f)); }
    glm::vec3 DebugCamera::GetForwardDirection() const { return glm::rotate(GetOrientationImpl(), glm::vec3(0.0f, 0.0f, -1.0f)); }

    float DebugCamera::RotationSpeed() const { return 0.8f; }
    float DebugCamera::PanSpeed() const { return 0.03f; }
    float DebugCamera::ZoomSpeed() const { return m_Distance * 0.2f; }
}
