#pragma once

#include "Components/Component.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"
#include "Events/MouseEvent.hpp"
#include <algorithm>
#include <imgui.h>

namespace NFSEngine
{
    class CameraController : public Component
    {
    public:
        CameraController(GameObject* owner) : Component(owner) {}

        std::string GetName() const override { return "CameraController"; }
        void SetTarget(Transform* target) { m_Target = target; }

        void OnEvent(Event& e)
        {
            EventDispatcher dispatcher(e);

            dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& ev) {
                m_Distance -= ev.GetYOffset() * m_ZoomSpeed;
                m_Distance = std::clamp(m_Distance, 2.0f, 40.0f);
                return true;
            });

            dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& ev) {
                if (!ImGui::GetIO().WantCaptureMouse)
                {
                    if (ev.GetMouseButton() == Mouse::ButtonLeft || ev.GetMouseButton() == Mouse::ButtonRight)
                    {
                        Application::Get().GetWindow().SetCursorMode(CursorMode::Locked);
                    }
                }
                return false;
            });
        }

    protected:
        virtual void OnStart() override
        {
            Application::Get().GetWindow().SetCursorMode(CursorMode::Locked);
        }

        virtual void OnUpdate(DeltaTime deltaTime) override
        {
            Window& window = Application::Get().GetWindow();

            if (Input::IsKeyDown(Key::Tab))
            {
                if (window.GetCursorMode() == CursorMode::Locked)
                    window.SetCursorMode(CursorMode::Normal);
                else
                    window.SetCursorMode(CursorMode::Locked);
            }

            if (window.GetCursorMode() == CursorMode::Locked)
            {
                float mouseX = Input::GetMouseX();
                float mouseY = Input::GetMouseY();

                if (m_FirstFrame) {
                    m_LastMouseX = mouseX; m_LastMouseY = mouseY;
                    m_FirstFrame = false;
                }

                m_Yaw += (mouseX - m_LastMouseX) * m_Sensitivity;
                m_Pitch = std::clamp(m_Pitch - (mouseY - m_LastMouseY) * m_Sensitivity, -45.0f, 85.0f);

                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
            }

            UpdateCameraTransform();
        }

    private:
        void UpdateCameraTransform()
        {
            if (!m_Target) return;

            float yawRad = glm::radians(m_Yaw);
            float pitchRad = glm::radians(m_Pitch);

            glm::vec3 offset = {
                m_Distance * cos(pitchRad) * cos(yawRad),
                m_Distance * sin(pitchRad),
                m_Distance * cos(pitchRad) * sin(yawRad)
            };

            auto* p_Transform = m_Owner->GetTransform();
            p_Transform->SetPosition(m_Target->GetPosition() + offset);

            glm::mat4 lookAt = glm::lookAt(p_Transform->GetPosition(), m_Target->GetPosition(), { 0, 1, 0 });
            p_Transform->SetRotation(glm::degrees(glm::eulerAngles(glm::quat_cast(glm::inverse(lookAt)))));
        }

        Transform* m_Target = nullptr;
        float m_Distance = 12.0f;
        float m_ZoomSpeed = 0.8f;
        float m_Yaw = 90.0f, m_Pitch = 25.0f;
        float m_Sensitivity = 0.12f;
        float m_LastMouseX = 0, m_LastMouseY = 0;
        bool m_FirstFrame = true;
    };
}