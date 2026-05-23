#pragma once

#include "Components/Component.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Core/InputAction.hpp"
#include "Core/Application.hpp"
#include "Events/MouseEvent.hpp"
#include "Core/Physics/PhysicsSystem.hpp"
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

        void SetCameraOverride(float yaw, float pitch, float distance) {
            if (!m_OverrideActive) {
                m_OriginalYaw = m_Yaw;
                m_OriginalPitch = m_Pitch;
                m_OriginalDistance = m_Distance;
            }
            m_ReturningToOriginal = false;
            m_OverrideActive = true;
            m_OverrideYaw = yaw;
            m_OverridePitch = pitch;
            m_OverrideDistance = distance;
        }

        void ClearCameraOverride() {
            m_OverrideYaw = m_OriginalYaw;
            m_OverridePitch = m_OriginalPitch;
            m_OverrideDistance = m_OriginalDistance;
            m_ReturningToOriginal = true;
        }

        void ResetMouseDelta() { m_FirstFrame = true; }

        void OnEvent(Event& e) {
            EventDispatcher dispatcher(e);

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
            float dt = static_cast<float>(deltaTime);

            if (InputActionManager::IsDown("ToggleCursor")) {
                if (window.GetCursorMode() == CursorMode::Locked)
                    window.SetCursorMode(CursorMode::Normal);
                else
                    window.SetCursorMode(CursorMode::Locked);
            }

            bool cursorLocked = (window.GetCursorMode() == CursorMode::Locked);

            if (dt > 0.2f) {
                m_FirstFrame = true;
            }

            if (cursorLocked) {
                float mouseX = Input::GetMouseX();
                float mouseY = Input::GetMouseY();

                if (!m_OverrideActive && !m_FirstFrame) {
                    m_Yaw += (mouseX - m_LastMouseX) * m_Sensitivity;
                    m_Pitch = std::clamp(m_Pitch + ((mouseY - m_LastMouseY) * m_Sensitivity), -45.0f, 85.0f);
                }

                m_FirstFrame = false;
                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
            } else {
                m_FirstFrame = true;
            }

            if (m_OverrideActive) {
                float yawDiff = m_OverrideYaw - m_Yaw;
                if (yawDiff > 180.0f) yawDiff -= 360.0f;
                if (yawDiff < -180.0f) yawDiff += 360.0f;
                m_Yaw += yawDiff * std::min(1.0f, m_OverrideLerpSpeed * dt);
                m_Pitch = glm::mix(m_Pitch, m_OverridePitch, m_OverrideLerpSpeed * dt);
                m_Distance = m_OverrideDistance;

                if (m_ReturningToOriginal) {
                    float remainingYaw = m_OverrideYaw - m_Yaw;
                    if (remainingYaw > 180.0f) remainingYaw -= 360.0f;
                    if (remainingYaw < -180.0f) remainingYaw += 360.0f;
                    if (std::abs(remainingYaw) < 0.1f && std::abs(m_Pitch - m_OverridePitch) < 0.1f
                        && std::abs(m_Distance - m_OverrideDistance) < 0.1f) {
                        m_OverrideActive = false;
                        m_ReturningToOriginal = false;
                    }
                }
            } else {
                float lookX = InputActionManager::GetFloat("LookX");
                float lookY = InputActionManager::GetFloat("LookY");
                m_Yaw += lookX * m_ControllerSensitivity;
                m_Pitch = std::clamp(m_Pitch + lookY * m_ControllerSensitivity, -45.0f, 85.0f);
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
                    NFSEngine::Sphere cameraSphere;
                    cameraSphere.Center = testPoint;
                    cameraSphere.Radius = sphereRadius;

                    bool hit = false;
                    Transform* otherTransform = go->GetTransform();

                    if (collider->Type == ColliderType::Box) {
                        auto* box = static_cast<BoxCollider3DComponent*>(collider);
                        hit = CollisionDetector::CheckAABBSphere(PhysicsSystem::GetAABB(otherTransform, box), cameraSphere)
                                  .IsColliding;
                    } else if (collider->Type == ColliderType::Sphere) {
                        auto* sphere = static_cast<SphereCollider3DComponent*>(collider);
                        hit = CollisionDetector::CheckSphere(PhysicsSystem::GetSphere(otherTransform, sphere), cameraSphere)
                                  .IsColliding;
                    } else if (collider->Type == ColliderType::Capsule) {
                        auto* capsule = static_cast<CapsuleCollider3DComponent*>(collider);
                        hit = CollisionDetector::CheckCapsuleSphere(PhysicsSystem::GetCapsule(otherTransform, capsule),
                                                                    cameraSphere)
                                  .IsColliding;
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
        float m_Yaw = 90.0f, m_Pitch = 25.0f;
        float m_Sensitivity = 0.12f;
        float m_ControllerSensitivity = 2.5f;
        float m_LastMouseX = 0, m_LastMouseY = 0;
        bool m_FirstFrame = true;

        bool m_OverrideActive = false;
        float m_OverrideYaw = 0.0f;
        float m_OverridePitch = 0.0f;
        float m_OverrideDistance = 0.0f;
        float m_OverrideLerpSpeed = 5.0f;
        float m_OriginalYaw = 90.0f;
        float m_OriginalPitch = 25.0f;
        float m_OriginalDistance = 12.0f;
        bool m_ReturningToOriginal = false;
    };
} // namespace NFSEngine