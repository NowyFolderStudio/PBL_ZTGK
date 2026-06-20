#include "Components/CharacterAnimationController.hpp"
#include "Components/CharacterController.hpp"
#include "Components/CoinComponent.hpp"
#include "Components/ParticleEmitterComponent.hpp"
#include "Components/PointLight.hpp"
#include "Core/Log.hpp"

void CharacterAnimationController::OnStart() {
    m_Rigidbody = m_Owner->GetTransform()->GetParent()->GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
    m_Animator = m_Owner->GetComponent<NFSEngine::AnimatorComponent>();
    m_Controller = m_Owner->GetTransform()->GetParent()->GetOwner()->GetComponent<CharacterController>();
    m_MaxSpeed = m_Controller->MaxSpeed;
    m_ParticleEmitter = m_Owner->GetComponent<NFSEngine::ParticleEmitterComponent>();
}

void CharacterAnimationController::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    m_WalkParticlesTimer += deltaTime;
    UpdateStates();
    ChangeAnimation();
}

void CharacterAnimationController::UpdateStates() {
    bool onGround = m_OnGround;
    m_OnGround = m_Rigidbody->IsGrounded;
    m_OnWall = IsTouchingJumpableWall();
    m_InMotion = (glm::length(m_Rigidbody->Velocity) > 1.0f);
    m_VerticalSpeed = glm::length(glm::vec2(m_Rigidbody->Velocity.x, m_Rigidbody->Velocity.z));
    float horizontalSpeed = m_HorizontalSpeed;
    m_HorizontalSpeed = m_Rigidbody->Velocity.y;
    if (onGround != m_OnGround && glm::abs(horizontalSpeed) > 5) {
        EmitJumpParticles(m_JumpParticlesCount);
    }
    if (onGround != m_OnGround && onGround && m_HorizontalSpeed > 0) {
        EmitJumpParticles(m_JumpParticlesCount);
    }
    if (!m_OnGround && m_HorizontalSpeed > horizontalSpeed) {
        EmitJumpParticles(m_JumpParticlesCount);
    }
}
void CharacterAnimationController::ChangeAnimation() {
    // Character animation indexes:
    // 0 - idle
    // 1 - run
    // 2 - jump
    // 3 - fall
    // 4 - WallJump
    // 5 - Dash
    m_Animator->SetAnimationSpeed(1.0f);
    glm::vec3 eulerDegrees = { 0, 0, 0 };
    m_Owner->GetTransform()->SetRotation(eulerDegrees);

    if (m_Controller->IsDashing()) {
        m_Animator->PlayAnimationBlended(5, 0.05f, false);
        return;
    }

    if (m_OnGround) {
        if (m_InMotion) {
            m_Animator->PlayAnimationBlended(1, 0.1f, true);
            m_Animator->SetAnimationSpeed(m_VerticalSpeed / m_MaxSpeed);
            if (m_VerticalSpeed > (m_MaxSpeed * 0.75f)) EmitWalkParticles();
        } else {
            m_Animator->PlayAnimationBlended(0, 0.1f, true);
        }
    } else {
        if (m_OnWall) {
            m_Animator->PlayAnimation(4, false);
            eulerDegrees = { 0, 90, 0 };
            m_Owner->GetTransform()->SetRotation(eulerDegrees);
        } else if (m_HorizontalSpeed > 0) {
            m_Animator->PlayAnimationBlended(2, 0.075f, false);
        } else {
            m_Animator->PlayAnimationBlended(3, 0.075f, false);
        }
    }
}

void CharacterAnimationController::EmitJumpParticles(int count) {
    m_ParticleEmitter->EmitMultiple(m_JumpParticleProperties, count);
}

void CharacterAnimationController::EmitWalkParticles() {
    if (m_WalkParticlesTimer > m_WalkParticlesTime) {
        m_ParticleEmitter->EmitMultiple(m_WalkParticleProperties, m_WalkParticlesCount);
        m_WalkParticlesTimer = 0;
    }
}
