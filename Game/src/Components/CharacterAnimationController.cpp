#include "Components/CharacterAnimationController.hpp"
#include "Components/CharacterController.hpp"

void CharacterAnimationController::OnStart() {
    m_Rigidbody = m_Owner->GetTransform()->GetParent()->GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
    m_Animator = m_Owner->GetComponent<NFSEngine::AnimatorComponent>();
    m_MaxSpeed = m_Owner->GetTransform()->GetParent()->GetOwner()->GetComponent<CharacterController>()->MaxSpeed;
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
    m_InMotion = (glm::length(m_Rigidbody->Velocity) > 0.01f);
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
    m_Animator->SetAnimationSpeed(1);
    if (m_OnGround) {
        if (m_InMotion) {
            m_Animator->PlayAnimation(1);
            m_Animator->SetAnimationSpeed(m_VerticalSpeed / m_MaxSpeed);
            if (m_VerticalSpeed > (m_MaxSpeed * 0.75)) EmitWalkParticles();
        } else {
            m_Animator->PlayAnimation(0);
        }
    } else {
        if (m_HorizontalSpeed > 0)
            m_Animator->PlayAnimation(2, false);
        else
            m_Animator->PlayAnimation(3, false);
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
