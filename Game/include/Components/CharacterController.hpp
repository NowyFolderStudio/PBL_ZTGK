#pragma once
#include <NFSEngine.h>
#include <memory>
#include <vector>
#include "Components/AnimatorComponent.hpp"
#include "Components/Camera.hpp"
#include "BounceComponent.hpp"
#include "Core/Audio/AudioClip.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Core/Log.hpp"
#include "Core/MathUtils.hpp"
#include "Aura/AuraManager.hpp"
#include "Managers/LivesManager.hpp"

class CharacterController : public NFSEngine::Component {
public:
    CharacterController(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { };

    std::string GetName() const override { return "CharacterController"; };

    // Run settings
    float MaxSpeed = 18.0f;
    float Acceleration = 80.0f;
    float Deceleration = 80.0f;
    float AirControl = 0.5f;
    float AirFriction = 0.5f;

    // Rotation settings
    float TurnSmoothTime = 0.1f;

    // Jump settings
    float JumpHeight = 5.0f;
    float FallMultiplier = 2.0f;
    float LowJumpMultiplier = 3.0f;
    int MaxJumps = 2;

    // Wall jump settings
    float WallSlideSpeed = 2.0f;
    float WallJumpPushForce = 15.0f;
    float WallJumpLockTime = 0.2f;

    // Dash settings
    float DashSpeed = 25.0f;
    float DashUpwardForce = 12.0f;
    float DashDuration = 0.4f;
    float DashGravityScale = 0.2f;
    float DashAirControl = 0.8f;

    // Coyote time and jump buffer settings
    float CoyoteTime = 0.15f;
    float JumpBufferTime = 0.15f;
    float WallCoyoteTime = 0.15f;

    float InvulnerabilityDuration = 2.0f;
    glm::vec3 SpawnPosition = glm::vec3(0.0f, 2.0f, 0.0f);

    void Respawn() {
        m_Owner->GetTransform()->SetPosition(SpawnPosition);
        if (p_RigidBody) {
            p_RigidBody->Velocity = glm::vec3(0.0f);
            p_RigidBody->IsGrounded = false;
        }

        m_JumpsRemaining = MaxJumps;
        m_IsJumping = false;
        m_IsDashing = false;
        m_CanDash = true;
        m_DashTimeCounter = 0.0f;
        m_CoyoteTimeCounter = 0.0f;
        m_JumpBufferCounter = 0.0f;
        m_WallCoyoteCounter = 0.0f;
        m_WallJumpLockCounter = 0.0f;
        m_LastWallNormal = glm::vec3(0.0f);
        m_LastJumpedWallNormal = glm::vec3(0.0f);
        m_IsInvulnerable = true;
        m_InvulnerabilityTimer = InvulnerabilityDuration;
    }

    bool IsInvulnerable() const { return m_IsInvulnerable; }

    bool TakeDamage(glm::vec3 damageSourcePosition, float knockbackStrength) {
        if (m_IsInvulnerable) return false;

        if (LivesManager::Instance) {
            LivesManager::Instance->LoseHeart();
        }

        m_IsInvulnerable = true;
        m_InvulnerabilityTimer = InvulnerabilityDuration;

        if (p_RigidBody) {
            glm::vec3 myPos = m_Owner->GetTransform()->GetWorldPosition();

            glm::vec3 knockbackDir = glm::normalize(myPos - damageSourcePosition);
            knockbackDir.y = 0.5f;

            p_RigidBody->Velocity = knockbackDir * knockbackStrength;

            m_IsDashing = false;
        }

        return true;
    }

    bool IsDashing() const { return m_IsDashing; }

private:
    NFSEngine::RigidBody3DComponent* p_RigidBody = nullptr;
    NFSEngine::Transform* m_CameraTransform = nullptr;
    std::vector<std::shared_ptr<NFSEngine::AudioClip>> m_JumpClips;
    std::shared_ptr<NFSEngine::AudioClip> m_DashClip;

    size_t m_AuraEventId = 0;
    bool m_IsDashUnlocked = false;

    glm::vec3 m_InputDirection = glm::vec3(0.0f);

    bool m_IsJumpPressed = false;
    bool m_IsDashPressed = false;

    bool m_IsJumping = false;
    bool m_IsDashing = false;
    bool m_CanDash = true;

    int m_JumpsRemaining = 0;
    float m_CoyoteTimeCounter = 0.0f;
    float m_JumpBufferCounter = 0.0f;
    float m_DashTimeCounter = 0.0f;
    float m_WallCoyoteCounter = 0.0f;
    float m_WallJumpLockCounter = 0.0f;

    bool m_IsInvulnerable = false;
    float m_InvulnerabilityTimer = 0.0f;

    glm::vec3 m_LastWallNormal = glm::vec3(0.0f);
    glm::vec3 m_LastJumpedWallNormal = glm::vec3(0.0f);

    float m_CurrentYaw = 0.0f;
    float m_TurnSmoothVelocity = 0.0f;

protected:
    void OnAwake() override {
        SpawnPosition = m_Owner->GetTransform()->GetPosition();
        m_JumpClips.push_back(std::make_shared<NFSEngine::AudioClip>("assets/audio/player_sounds/jump1.mp3"));
        m_JumpClips.push_back(std::make_shared<NFSEngine::AudioClip>("assets/audio/player_sounds/jump2.mp3"));
        m_JumpClips.push_back(std::make_shared<NFSEngine::AudioClip>("assets/audio/player_sounds/jump4.mp3"));
        m_DashClip = std::make_shared<NFSEngine::AudioClip>("assets/audio/player_sounds/dash.mp3");
    }

    virtual void OnStart() override {
        p_RigidBody = m_Owner->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!p_RigidBody) {
            NFS_CORE_ERROR("CharacterController: Brak RigidBody3D!");
        }

        const auto& sceneObjects = m_Owner->GetScene()->GetAllGameObjects();
        for (const auto& go : sceneObjects) {
            if (go->GetComponent<NFSEngine::Camera>()) {
                m_CameraTransform = go->GetTransform();
                break;
            }
        }

        if (AuraManager::Instance) {
            UpdateAbilities(AuraManager::Instance->CurrentAura);
            m_AuraEventId
                = AuraManager::Instance->OnAuraChanged.AddListener([this](AuraType newAura) { this->UpdateAbilities(newAura); });
        }
    }

    virtual void OnDisable() override {
        if (AuraManager::Instance && m_AuraEventId != 0) {
            AuraManager::Instance->OnAuraChanged.RemoveListener(m_AuraEventId);
            m_AuraEventId = 0;
        }
    }

    virtual void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        m_InputDirection.x = NFSEngine::InputActionManager::GetFloat("MoveX");
        m_InputDirection.z = NFSEngine::InputActionManager::GetFloat("MoveZ");

        m_IsJumpPressed = NFSEngine::InputActionManager::IsPressed("Jump");
        m_IsDashPressed = NFSEngine::InputActionManager::IsPressed("Dash");

        if (NFSEngine::InputActionManager::IsDown("Jump")) {
            Jump();
        }

        if (NFSEngine::InputActionManager::IsDown("Dash")) {
            Dash();
        }
    }

    virtual void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!p_RigidBody || !m_CameraTransform) return;
        float dt = static_cast<float>(deltaTime);

        UpdateStates();
        HandleCounters(dt);

        if (m_IsDashing) {
            HandleDash(dt);
        } else {
            HandleGravity(dt);
            HandleJump();
        }

        HandleRotation(dt);
        ApplyMovement(dt);

        HandleMovingPlatforms(dt);
    }

private:
    void UpdateStates() {
        if (p_RigidBody->IsGrounded) {

            m_LastJumpedWallNormal = glm::vec3(0.0f);

            if (!m_IsDashing) {
                m_CanDash = true;
            }

            if (p_RigidBody->Velocity.y <= 0.2f) {
                m_JumpsRemaining = MaxJumps;
                m_IsJumping = false;
            }
        }

        if (m_DashTimeCounter <= 0.0f || (p_RigidBody->IsGrounded && p_RigidBody->Velocity.y <= 0.2f)) {
            m_IsDashing = false;
        }
    }

    void HandleCounters(float dt) {
        if (p_RigidBody->IsGrounded) {
            m_CoyoteTimeCounter = CoyoteTime;
        } else {
            m_CoyoteTimeCounter -= dt;

            if (m_CoyoteTimeCounter <= 0.0f && m_JumpsRemaining == MaxJumps) {
                m_JumpsRemaining--;
            }
        }

        if (m_JumpBufferCounter > 0.0f) {
            m_JumpBufferCounter -= dt;
        }

        if (m_DashTimeCounter > 0.0f) {
            m_DashTimeCounter -= dt;
        }

        if (m_WallJumpLockCounter > 0.0f) {
            m_WallJumpLockCounter -= dt;
        }
        if (p_RigidBody->IsGrounded || IsTouchingJumpableWall()) {
            m_WallJumpLockCounter = 0.0f;
        }

        if (IsTouchingJumpableWall() && !p_RigidBody->IsGrounded) {
            m_LastWallNormal = p_RigidBody->WallNormal;
            m_WallCoyoteCounter = WallCoyoteTime;
        } else if (m_WallCoyoteCounter > 0.0f) {
            m_WallCoyoteCounter -= dt;
        }

        if (m_InvulnerabilityTimer > 0.0f) {
            m_InvulnerabilityTimer -= dt;
            if (m_InvulnerabilityTimer <= 0.0f) {
                m_IsInvulnerable = false;
            }
        }
    }

    glm::vec3 GetMovementVector() {
        if (m_InputDirection.x == 0.0f && m_InputDirection.z == 0.0f) {
            return glm::vec3(0.0f);
        }

        glm::vec3 camPos = m_CameraTransform->GetPosition();
        glm::vec3 playerPos = m_Owner->GetTransform()->GetPosition();

        glm::vec3 forward = playerPos - camPos;
        forward.y = 0.0f;
        forward = glm::normalize(forward);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        glm::vec3 rawInput = (forward * m_InputDirection.z) + (right * m_InputDirection.x);

        float inputMagnitude = glm::length(m_InputDirection);

        inputMagnitude = glm::clamp(inputMagnitude, 0.0f, 1.0f);

        if (glm::length(rawInput) > 0.001f) {
            return glm::normalize(rawInput) * inputMagnitude;
        }
        return glm::vec3(0.0f);
    }

    void HandleRotation(float dt) {
        glm::vec3 moveDir = GetMovementVector();

        if (m_WallJumpLockCounter > 0.0f) {
            moveDir = glm::vec3(p_RigidBody->Velocity.x, 0.0f, p_RigidBody->Velocity.z);
        }

        if (glm::length(moveDir) > 0.1f) {
            float targetAngle = glm::degrees(atan2(moveDir.x, moveDir.z));
            m_CurrentYaw = NFSEngine::Math::SmoothDampAngle(m_CurrentYaw, targetAngle, m_TurnSmoothVelocity, TurnSmoothTime, dt);
            m_Owner->GetTransform()->SetRotation(glm::vec3(0.0f, m_CurrentYaw, 0.0f));
        }
    }

    void Jump() { m_JumpBufferCounter = JumpBufferTime; }

    void HandleJump() {
        if (m_JumpBufferCounter > 0.0f) {
            bool jumped = false;

            if (m_WallCoyoteCounter > 0.0f && !p_RigidBody->IsGrounded) {

                if (m_IsDashing) m_IsDashing = false;
                ExecuteWallJump();
                jumped = true;
                PlayJumpSound();
            }

            if (!jumped && (m_CoyoteTimeCounter > 0.0f || m_JumpsRemaining > 0)) {
                if (m_IsDashing) m_IsDashing = false;
                ExecuteJump();
                PlayJumpSound();
            }
        }
    }

    void ExecuteJump() {
        p_RigidBody->Velocity.y = sqrt(2.0f * JumpHeight * -NFSEngine::PhysicsSystem::Gravity.y);
        m_IsJumping = true;
        m_JumpsRemaining--;

        m_JumpBufferCounter = 0.0f;
        m_CoyoteTimeCounter = 0.0f;
    }

    void ExecuteWallJump() {
        glm::vec3 jumpDirection = m_LastWallNormal;

        p_RigidBody->Velocity.y = sqrt(2.0f * JumpHeight * -NFSEngine::PhysicsSystem::Gravity.y);

        p_RigidBody->Velocity.x = jumpDirection.x * WallJumpPushForce;
        p_RigidBody->Velocity.z = jumpDirection.z * WallJumpPushForce;

        m_IsJumping = true;
        m_JumpsRemaining = MaxJumps - 1;

        m_JumpBufferCounter = 0.0f;
        m_CoyoteTimeCounter = 0.0f;

        m_WallJumpLockCounter = WallJumpLockTime;
    }

    void Dash() {
        if (!m_IsDashUnlocked) return;

        if (m_IsDashPressed && m_CanDash && !m_IsDashing) {

            m_IsDashing = true;
            m_CanDash = false;
            m_DashTimeCounter = DashDuration;

            glm::vec3 dashDirection = GetMovementVector();

            if (glm::length(dashDirection) < 0.1f) {
                dashDirection = glm::vec3(
                    0.0f, 0.0f, 1.0f); // TODO: Refactor this when camera transform will have method to get forward direction
            }

            p_RigidBody->Velocity.x = dashDirection.x * DashSpeed;
            p_RigidBody->Velocity.z = dashDirection.z * DashSpeed;
            p_RigidBody->Velocity.y = DashUpwardForce;

            m_IsJumping = true;
            PlayDashSound();
        }
    }

    void HandleDash(float dt) {
        float dashGravity = NFSEngine::PhysicsSystem::Gravity.y * DashGravityScale;
        p_RigidBody->Velocity.y += dashGravity * dt;
    }

    void ApplyMovement(float dt) {

        if (m_WallJumpLockCounter > 0.0f) {
            return;
        }

        glm::vec3 moveVector = GetMovementVector();

        float currentMaxSpeed = m_IsDashing ? DashSpeed : MaxSpeed;
        float currentAcc;
        float currentDec;

        if (p_RigidBody->IsGrounded) {
            currentAcc = Acceleration;
            currentDec = Deceleration;
        } else {
            currentAcc = m_IsDashing ? DashAirControl * Acceleration : AirControl * Acceleration;
            currentDec = m_IsDashing ? 0 : AirControl * Deceleration;
        }

        if (glm::length(m_InputDirection) > 0.1f) {
            glm::vec3 targetVel = moveVector * currentMaxSpeed;

            p_RigidBody->Velocity.x = NFSEngine::Math::MoveTowards(p_RigidBody->Velocity.x, targetVel.x, currentAcc * dt);
            p_RigidBody->Velocity.z = NFSEngine::Math::MoveTowards(p_RigidBody->Velocity.z, targetVel.z, currentAcc * dt);

        } else {
            if (p_RigidBody->IsGrounded) {

                p_RigidBody->Velocity.x = NFSEngine::Math::MoveTowards(p_RigidBody->Velocity.x, 0.0f, currentDec * dt);
                p_RigidBody->Velocity.z = NFSEngine::Math::MoveTowards(p_RigidBody->Velocity.z, 0.0f, currentDec * dt);
            } else {
                float dragMultiplier = glm::clamp(1.0f - (AirFriction * dt), 0.0f, 1.0f);

                if (m_IsDashing) dragMultiplier = glm::clamp(1.0f - (1.0f * dt), 0.0f, 1.0f);

                p_RigidBody->Velocity.x *= dragMultiplier;
                p_RigidBody->Velocity.z *= dragMultiplier;
            }
        }

        if (IsTouchingJumpableWall() && !p_RigidBody->IsGrounded) {

            if (glm::dot(moveVector, m_LastWallNormal) > 0.1f) {
            } else {
                glm::vec3 currentXZ = glm::vec3(p_RigidBody->Velocity.x, 0.0f, p_RigidBody->Velocity.z);

                float dotProduct = glm::dot(currentXZ, m_LastWallNormal);

                glm::vec3 allowedVelocityXZ = m_LastWallNormal * dotProduct;

                p_RigidBody->Velocity.x = allowedVelocityXZ.x;
                p_RigidBody->Velocity.z = allowedVelocityXZ.z;
            }
        }
    }

    void HandleGravity(float dt) {
        if (p_RigidBody->Velocity.y < 0.0f) {
            p_RigidBody->Velocity.y += NFSEngine::PhysicsSystem::Gravity.y * (FallMultiplier - 1.0f) * dt;

        } else if (p_RigidBody->Velocity.y > 0.0f && !m_IsJumpPressed) {
            p_RigidBody->Velocity.y += NFSEngine::PhysicsSystem::Gravity.y * (LowJumpMultiplier - 1.0f) * dt;
        }

        if (IsTouchingJumpableWall() && !p_RigidBody->IsGrounded && p_RigidBody->Velocity.y < 0.0f) {
            p_RigidBody->Velocity.y = NFSEngine::Math::Clamp(p_RigidBody->Velocity.y, -WallSlideSpeed, 0.0f);
        }
    }

    void HandleMovingPlatforms(float dt) {
        if (!p_RigidBody || !p_RigidBody->IsGrounded || !p_RigidBody->TouchedFloorObject) return;

        auto* floorRB = p_RigidBody->TouchedFloorObject->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!floorRB) return;

        auto* transform = m_Owner->GetTransform();

        if (glm::length(floorRB->Velocity) > 0.0001f) {
            transform->Move(floorRB->Velocity * dt);
        }

        if (glm::length(floorRB->AngularVelocity) > 0.0001f) {
            glm::vec3 playerPos = transform->GetWorldPosition();
            glm::vec3 floorPos = p_RigidBody->TouchedFloorObject->GetTransform()->GetWorldPosition();

            glm::vec3 radius = playerPos - floorPos;
            radius.y = 0.0f;

            glm::vec3 angularVelRad = glm::radians(floorRB->AngularVelocity);

            glm::vec3 tangentialVelocity = glm::cross(angularVelRad, radius);

            transform->Move(tangentialVelocity * dt);

            transform->Rotate(floorRB->AngularVelocity * dt);

            m_CurrentYaw += floorRB->AngularVelocity.y * dt;
        }
    }

    bool IsTouchingJumpableWall() const {
        return p_RigidBody->IsTouchingWall && p_RigidBody->TouchedWallObject != nullptr
            && p_RigidBody->TouchedWallObject->CompareTag(NFSEngine::Tags::WallJumpSurface);
    }

    void UpdateAbilities(AuraType currentAura) {
        int oldMaxJumps = MaxJumps;

        if (currentAura == AuraType::First) {
            MaxJumps = 2;
        } else {
            MaxJumps = 1;
        }

        m_JumpsRemaining += (MaxJumps - oldMaxJumps);

        if (m_JumpsRemaining < 0) {
            m_JumpsRemaining = 0;
        }
        if (m_JumpsRemaining > MaxJumps) {
            m_JumpsRemaining = MaxJumps;
        }

        if (currentAura == AuraType::Second) {
            m_IsDashUnlocked = true;
        } else {
            m_IsDashUnlocked = false;
        }
    }

    void PlayJumpSound() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, m_JumpClips.size() - 1);

        int clipIndex = dis(gen);

        NFSEngine::AudioEngine::PlayClipRandomPitch(m_JumpClips[clipIndex].get(), 0.95f, 1);
    }
    void PlayDashSound() { NFSEngine::AudioEngine::PlayClipRandomPitch(m_DashClip.get(), 0.95f, 1, 1.5f); }
};