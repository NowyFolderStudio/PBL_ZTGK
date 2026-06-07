#include "Components/CharacterAnimationController.hpp"
#include "Core/Log.hpp"

void CharacterAnimationController::OnStart() {
    m_Rigidbody = m_Owner->GetTransform()->GetParent()->GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();
    m_Animator = m_Owner->GetComponent<NFSEngine::AnimatorComponent>();
}

void CharacterAnimationController::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    UpdateStates();
    ChangeAnimation();
}

void CharacterAnimationController::UpdateStates() {
    m_OnGround = m_Rigidbody->IsGrounded;
    m_InMotion = (glm::length(m_Rigidbody->Velocity) > 0.01f);
    m_VerticalSpeed = glm::length(glm::vec2(m_Rigidbody->Velocity.x, m_Rigidbody->Velocity.z));
    m_HorizontalSpeed = m_Rigidbody->Velocity.y;
}
void CharacterAnimationController::ChangeAnimation() {
    // Character animation indexes:
    // 0 - idle
    // 1 - run
    // 2 - jump
    // 3 - fall
    if (m_OnGround) {
        if (m_InMotion)
            m_Animator->PlayAnimation(1);
        else
            m_Animator->PlayAnimation(0);
    } else {
        if (m_HorizontalSpeed > 0)
            m_Animator->PlayAnimation(2, false);
        else
            m_Animator->PlayAnimation(3, false);
    }
}
