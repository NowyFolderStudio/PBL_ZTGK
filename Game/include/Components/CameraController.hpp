#pragma once

#include "Components/Component.hpp"
#include "Components/Camera.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/Transform.hpp"
#include "Core/InputAction.hpp"
#include "Core/Application.hpp"
#include "Core/Tags.hpp"
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
            if (m_Target && m_Target->GetOwner()) {
                m_TargetRigidBody = m_Target->GetOwner()->GetComponent<RigidBody3DComponent>();
            } else {
                m_TargetRigidBody
                    = m_Owner->GetScene()->FindGameObjectsWithTag(Tags::Player)[0]->GetComponent<RigidBody3DComponent>();
            }
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

            bool hasManualInput = false; // Flaga sprawdzająca, czy gracz rusza kamerą

            if (cursorLocked) {
                float mouseX = Input::GetMouseX();
                float mouseY = Input::GetMouseY();

                if (!m_OverrideActive && !m_FirstFrame) {
                    float deltaX = mouseX - m_LastMouseX;
                    float deltaY = mouseY - m_LastMouseY;

                    if (std::abs(deltaX) > 0.01f || std::abs(deltaY) > 0.01f) {
                        m_Yaw += deltaX * m_Sensitivity;
                        m_Pitch = std::clamp(m_Pitch + (deltaY * m_Sensitivity), -45.0f, 85.0f);
                        hasManualInput = true;
                    }
                }

                m_FirstFrame = false;
                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
            } else {
                m_FirstFrame = true;
            }

            if (!m_OverrideActive) {
                float lookX = InputActionManager::GetFloat("LookX");
                float lookY = InputActionManager::GetFloat("LookY");

                if (std::abs(lookX) > 0.01f || std::abs(lookY) > 0.01f) {
                    m_Yaw += lookX * m_ControllerSensitivity;
                    m_Pitch = std::clamp(m_Pitch + lookY * m_ControllerSensitivity, -45.0f, 85.0f);
                    hasManualInput = true;
                }
            }

            if (!m_OverrideActive && m_AutoCenterEnabled) {
                if (hasManualInput) {
                    m_TimeSinceLastInput = 0.0f;
                } else {
                    m_TimeSinceLastInput += dt;
                }

                if (m_TimeSinceLastInput >= m_AutoCenterDelay && m_TargetRigidBody) {
                    glm::vec3 flatVelocity = glm::vec3(m_TargetRigidBody->Velocity.x, 0.0f, m_TargetRigidBody->Velocity.z);

                    if (glm::length(flatVelocity) > m_VelocityThreshold) {
                        float targetYawRad = atan2(-flatVelocity.z, -flatVelocity.x);
                        float targetYawDeg = glm::degrees(targetYawRad);

                        float angleDiff = targetYawDeg - m_Yaw;
                        while (angleDiff > 180.0f)
                            angleDiff -= 360.0f;
                        while (angleDiff < -180.0f)
                            angleDiff += 360.0f;

                        if (std::abs(angleDiff) > 135.0f) angleDiff = 0.0f;

                        float speedFactor = glm::clamp(glm::length(flatVelocity) / 10.0f, 0.5f, 2.0f);
                        m_Yaw += angleDiff * m_AutoCenterSpeed * speedFactor * dt;
                    }
                }
            }

            if (m_OverrideActive) {
                float yawDiff = m_OverrideYaw - m_Yaw;

                while (yawDiff > 180.0f)
                    yawDiff -= 360.0f;
                while (yawDiff < -180.0f)
                    yawDiff += 360.0f;

                m_Yaw += yawDiff * std::min(1.0f, m_OverrideLerpSpeed * dt);
                m_Pitch = glm::mix(m_Pitch, m_OverridePitch, m_OverrideLerpSpeed * dt);

                if (m_ReturningToOriginal) {
                    m_Distance = glm::mix(m_Distance, m_OriginalDistance, m_OverrideLerpSpeed * dt);
                } else {
                    m_Distance = m_OverrideDistance;
                }

                if (m_ReturningToOriginal) {
                    float remainingYaw = m_OriginalYaw - m_Yaw;

                    while (remainingYaw > 180.0f)
                        remainingYaw -= 360.0f;
                    while (remainingYaw < -180.0f)
                        remainingYaw += 360.0f;

                    if (std::abs(remainingYaw) < 0.1f && std::abs(m_Pitch - m_OriginalPitch) < 0.1f
                        && std::abs(m_Distance - m_OriginalDistance) < 0.1f) {
                        m_OverrideActive = false;
                        m_ReturningToOriginal = false;
                        m_Distance = m_OriginalDistance;
                    }
                }
            }

            UpdateCameraTransform(deltaTime);
        }

    private:
        void UpdateCameraTransform(DeltaTime dt) {
            if (!m_Target) return;
            float dtFloat = static_cast<float>(dt);

            glm::vec3 playerPos = m_Target->GetPosition();
            glm::vec3 targetPos = playerPos + glm::vec3(0.0f, 1.5f, 0.0f);

            if (!m_FocusInitialized) {
                m_FocusPoint = targetPos;
                m_FocusInitialized = true;
            }

            m_FocusPoint.x = targetPos.x;
            m_FocusPoint.z = targetPos.z;

            if (m_FocusPoint.y < targetPos.y - m_VerticalDeadzoneUp) {
                m_FocusPoint.y = targetPos.y - m_VerticalDeadzoneUp;
            } else if (m_FocusPoint.y > targetPos.y + m_VerticalDeadzoneDown) {
                m_FocusPoint.y = targetPos.y + m_VerticalDeadzoneDown;
            }

            if (m_CenterOnGround && m_TargetRigidBody && m_TargetRigidBody->IsGrounded) {
                m_FocusPoint.y = glm::mix(m_FocusPoint.y, targetPos.y, m_VerticalSmoothSpeed * dtFloat);
            }

            float yawRad = glm::radians(m_Yaw);
            float pitchRad = glm::radians(m_Pitch);

            glm::vec3 direction = { cos(pitchRad) * cos(yawRad), sin(pitchRad), cos(pitchRad) * sin(yawRad) };

            glm::vec3 desiredCameraPos = m_FocusPoint + direction * m_Distance;

            glm::vec3 rayOrigin = targetPos;
            glm::vec3 rayDirection = glm::normalize(desiredCameraPos - rayOrigin);
            float distanceToCamera = glm::length(desiredCameraPos - rayOrigin);

            float collisionDistance = CheckCameraCollision(rayOrigin, rayDirection, distanceToCamera);

            float targetDistanceForLerp;

            if (collisionDistance < distanceToCamera) {
                glm::vec3 hitPoint = rayOrigin + rayDirection * collisionDistance;
                glm::vec3 vectorToHit = hitPoint - m_FocusPoint;

                targetDistanceForLerp = glm::dot(vectorToHit, direction);
                targetDistanceForLerp = std::max(0.5f, targetDistanceForLerp);
            } else {
                targetDistanceForLerp = m_Distance;
            }

            if (m_CurrentDistance < targetDistanceForLerp) {
                m_CurrentDistance = glm::mix(m_CurrentDistance, targetDistanceForLerp, 15.0f * dtFloat);
            } else {
                m_CurrentDistance = targetDistanceForLerp;
            }

            auto* pTransform = m_Owner->GetTransform();

            pTransform->SetPosition(m_FocusPoint + direction * m_CurrentDistance);

            glm::mat4 lookAt = glm::lookAt(pTransform->GetPosition(), m_FocusPoint, { 0, 1, 0 });
            pTransform->SetRotation(glm::degrees(glm::eulerAngles(glm::quat_cast(glm::inverse(lookAt)))));
        }

        [[nodiscard]] float CheckCameraCollision(const glm::vec3& targetPos, const glm::vec3& direction,
                                                 float maxDistance) const {
            if (m_OverrideActive && !m_ReturningToOriginal) return maxDistance;

            Ray ray { targetPos, direction };
            RaycastResult hit;

            RaycastOptions options;
            options.MaxDistance = maxDistance;
            options.IgnoreTriggers = true;
            options.TagsToIgnore = Tags::Player;

            const auto& colliders = m_Owner->GetScene()->GetAllColliders();

            if (PhysicsSystem::Raycast(ray, hit, colliders, options)) {
                return std::max(0.5f, hit.Distance - 0.4f);
            }

            return maxDistance;
        }

        Transform* m_Target = nullptr;
        float m_Distance = 20.0f;
        float m_CurrentDistance = 20.0f;
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
        float m_OriginalDistance = 20.0f;
        bool m_ReturningToOriginal = false;

        bool m_AutoCenterEnabled = true;
        float m_AutoCenterSpeed = 1.0f;
        float m_AutoCenterDelay = 0.2f;
        float m_VelocityThreshold = 2.0f;

        NFSEngine::RigidBody3DComponent* m_TargetRigidBody = nullptr;
        float m_TimeSinceLastInput = 0.0f;

        glm::vec3 m_FocusPoint = glm::vec3(0.0f);
        bool m_FocusInitialized = false;

        float m_VerticalDeadzoneUp = 5.0f;
        float m_VerticalDeadzoneDown = 2.5f;

        float m_VerticalSmoothSpeed = 5.0f;
        bool m_CenterOnGround = true;
    };
} // namespace NFSEngine