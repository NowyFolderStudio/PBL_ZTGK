#include "Components/CharacterController.hpp"
#include "Components/ParticleEmitterComponent.hpp"
#include "NFSEngine.h"
#include "Renderer/Particle.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

class CharacterAnimationController : public NFSEngine::Component {
public:
    CharacterAnimationController(NFSEngine::GameObject* owner)
        : Component(owner) {
        m_JumpParticleProperties.lifeTime = 0.6f;
        m_JumpParticleProperties.colorBegin = glm::vec4(1, 1, 1, 1);
        m_JumpParticleProperties.colorEnd = glm::vec4(0.5, 0.5, 0.5, 0.75);
        m_JumpParticleProperties.velocity = { 0, 1, 0 };
        m_JumpParticleProperties.velocityVariation = { 6, 1, 6 };
        m_JumpParticleProperties.sizeBegin = 1.5;
        m_JumpParticleProperties.sizeEnd = 0;
        m_JumpParticleProperties.rotationVariation = 90;
        m_JumpParticleProperties.rotationSpeedVariation = 30;

        m_WalkParticleProperties.lifeTime = 0.3f;
        m_WalkParticleProperties.colorBegin = glm::vec4(1, 1, 1, 1);
        m_WalkParticleProperties.colorEnd = glm::vec4(0.5, 0.5, 0.5, 0.75);
        m_WalkParticleProperties.velocity = { 0, 1, 0 };
        m_WalkParticleProperties.velocityVariation = { 4, 1, 4 };
        m_WalkParticleProperties.sizeBegin = 0.8;
        m_WalkParticleProperties.sizeEnd = 0;
        m_WalkParticleProperties.rotationVariation = 90;
        m_WalkParticleProperties.rotationSpeedVariation = 20;
    }
    std::string GetName() const override { return "CharacterAnimationController"; }

protected:
    void OnStart() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void UpdateStates();
    void ChangeAnimation();

private:
    bool m_OnGround = false;
    bool m_InMotion = false;
    bool m_OnWall = false;
    float m_VerticalSpeed = 0.0f;
    float m_HorizontalSpeed = 0.0f;
    float m_MaxSpeed = 0.0f;
    NFSEngine::RigidBody3DComponent* m_Rigidbody;
    NFSEngine::AnimatorComponent* m_Animator;
    CharacterController* m_Controller;
    NFSEngine::ParticleEmitterComponent* m_ParticleEmitter;
    NFSEngine::ParticleProperties m_JumpParticleProperties;
    NFSEngine::ParticleProperties m_WalkParticleProperties;
    int m_JumpParticlesCount = 10;

    int m_WalkParticlesCount = 3;
    float m_WalkParticlesTime = 0.1f;
    float m_WalkParticlesTimer = 0;

    void EmitJumpParticles(int count);
    void EmitWalkParticles();

    bool IsTouchingJumpableWall() const {
        return m_Rigidbody->IsTouchingWall && m_Rigidbody->TouchedWallObject != nullptr
            && m_Rigidbody->TouchedWallObject->CompareTag(NFSEngine::Tags::WallJumpSurface);
    }
};