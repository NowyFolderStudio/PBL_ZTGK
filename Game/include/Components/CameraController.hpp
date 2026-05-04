#pragma once

#include "Components/Component.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"
#include "Events/MouseEvent.hpp"
#include "Core/PhysicsSystem.hpp"
#include "Core/Scene.hpp"
#include <algorithm>
#include <imgui.h>

namespace NFSEngine {
    class CameraController : public Component {
    public:
        explicit CameraController(GameObject* owner)
            : Component(owner) { }

        [[nodiscard]] std::string GetName() const override { return "CameraController"; }
        void SetTarget(Transform* target) { m_Target = target; }

        void OnEvent(Event& e) {
            EventDispatcher dispatcher(e);

            dispatcher.Dispatch<MouseScrolledEvent>([this](const MouseScrolledEvent& ev) {
                m_Distance -= ev.GetYOffset() * m_ZoomSpeed;
                m_Distance = std::clamp(m_Distance, m_MinDistance, m_MaxDistance);
                return true;
            });
            dispatcher.Dispatch<MouseButtonPressedEvent>([](const MouseButtonPressedEvent& ev) {
                if (!ImGui::GetIO().WantCaptureMouse) {
                    if (ev.GetMouseButton() == Mouse::ButtonLeft || ev.GetMouseButton() == Mouse::ButtonRight) {
                        Application::Get().GetWindow().SetCursorMode(CursorMode::Locked);
                    }
                }
                return false;
            });
        }

    protected:
        void OnStart() override {
            Application::Get().GetWindow().SetCursorMode(CursorMode::Locked);
            m_CurrentDistance = m_Distance;
        }

        void OnUpdate(DeltaTime deltaTime) override {
            Window& window = Application::Get().GetWindow();

            if (Input::IsKeyDown(Key::Tab)) {
                if (window.GetCursorMode() == CursorMode::Locked)
                    window.SetCursorMode(CursorMode::Normal);
                else
                    window.SetCursorMode(CursorMode::Locked);
            }

            if (window.GetCursorMode() == CursorMode::Locked) {
                float mouseX = Input::GetMouseX();
                float mouseY = Input::GetMouseY();

                if (m_FirstFrame) {
                    m_LastMouseX = mouseX;
                    m_LastMouseY = mouseY;
                    m_FirstFrame = false;
                }

                m_Yaw += (mouseX - m_LastMouseX) * m_Sensitivity;
                m_Pitch = std::clamp(m_Pitch + ((mouseY - m_LastMouseY) * m_Sensitivity), -45.0f, 85.0f);

                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
            }

            UpdateCameraTransform(deltaTime);
        }

    private:
        void UpdateCameraTransform(DeltaTime dt) {
            if (!m_Target) return;

            float yawRad = glm::radians(m_Yaw);
            float pitchRad = glm::radians(m_Pitch);

            glm::vec3 direction = { cos(pitchRad) * cos(yawRad), sin(pitchRad), cos(pitchRad) * sin(yawRad) };

            float collisionDistance = CheckCameraCollision(m_Target->GetPosition(), direction);
            m_CurrentDistance = glm::mix(m_CurrentDistance, collisionDistance, 15.0f * static_cast<float>(dt));

            auto* pTransform = m_Owner->GetTransform();
            pTransform->SetPosition(m_Target->GetPosition() + direction * m_CurrentDistance);

            glm::mat4 lookAt = glm::lookAt(pTransform->GetPosition(), m_Target->GetPosition(), { 0, 1, 0 });
            pTransform->SetRotation(glm::degrees(glm::eulerAngles(glm::quat_cast(glm::inverse(lookAt)))));
        }

        [[nodiscard]] float CheckCameraCollision(const glm::vec3& targetPos, const glm::vec3& direction) const {
            float minHitDist = m_Distance;

            Scene* scene = m_Owner->GetScene();
            if (!scene) return m_Distance;

            for (const auto& go : scene->GetAllGameObjects()) {
                if (go.get() == m_Owner || go->GetTransform() == m_Target) continue;

                auto* collider = go->GetComponent<ColliderComponent>();
                if (!collider || collider->IsTrigger) continue;

                int steps = 8;
                for (int i = 1; i <= steps; ++i) {
                    float t = static_cast<float>(i) / static_cast<float>(steps);
                    float testDist = m_Distance * t;
                    glm::vec3 testPoint = targetPos + direction * testDist;

                    float sphereRadius = 0.4f;
                    PhysicsSystem::Sphere cameraSphere;
                    cameraSphere.Center = testPoint;
                    cameraSphere.Radius = sphereRadius;

                    bool hit = false;
                    Transform* otherTransform = go->GetTransform();

                    if (collider->Type == ColliderType::Box) {
                        auto* box = static_cast<BoxCollider3DComponent*>(collider);
                        hit = PhysicsSystem::MathCheckAABBSphere(PhysicsSystem::GetAABB(otherTransform, box), cameraSphere);
                    } else if (collider->Type == ColliderType::Sphere) {
                        auto* sphere = static_cast<SphereCollider3DComponent*>(collider);
                        hit = PhysicsSystem::MathCheckSphere(PhysicsSystem::GetSphere(otherTransform, sphere), cameraSphere);
                    } else if (collider->Type == ColliderType::Capsule) {
                        auto* capsule = static_cast<CapsuleCollider3DComponent*>(collider);
                        hit = PhysicsSystem::MathCheckCapsuleSphere(PhysicsSystem::GetCapsule(otherTransform, capsule),
                                                                    cameraSphere);
                    }

                    if (hit) {
                        float safeDist = std::max(0.5f, testDist - 0.2f);
                        minHitDist = std::min(safeDist, minHitDist);
                        break;
                    }
                }
            }

            return minHitDist;
        }

        Transform* m_Target = nullptr;
        float m_Distance = 12.0f;
        float m_CurrentDistance = 12.0f;
        float m_MinDistance = 2.0f;
        float m_MaxDistance = 40.0f;
        float m_ZoomSpeed = 0.8f;
        float m_Yaw = 90.0f, m_Pitch = 25.0f;
        float m_Sensitivity = 0.12f;
        float m_LastMouseX = 0, m_LastMouseY = 0;
        bool m_FirstFrame = true;
    };
} // namespace NFSEngine