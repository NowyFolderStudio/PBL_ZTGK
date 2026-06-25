#pragma once
#include "Core/Audio/AudioClip.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include <NFSEngine.h>
#include <algorithm>
#include <memory>

class BounceComponent : public NFSEngine::Component {
public:
    BounceComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "BounceComponent"; }

    float BaseBounceHeight = 15.0f;
    float MaxBounceHeight = 640.0f;
    float BounceMultiplier = 0.8f;

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();

        if (collider) {
            collider->IsTrigger = false;

            collider->OnCollisionEnter = [this](NFSEngine::GameObject* other, glm::vec3 contactNormal) {
                if (other->CompareTag(NFSEngine::Tags::Player)) {
                    this->ApplyBounce(other, contactNormal);
                }
            };
        }
        m_AudioClip = std::make_shared<NFSEngine::AudioClip>("assets/audio/sounds/snare03.ogg");
    }

private:
    std::shared_ptr<NFSEngine::AudioClip> m_AudioClip;
    void ApplyBounce(NFSEngine::GameObject* targetObject, glm::vec3 contactNormal) {
        auto* rb = targetObject->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!rb) return;

        NFSEngine::AudioEngine::PlayClip(m_AudioClip.get());

        glm::vec3 bounceUp = m_Owner->GetTransform()->GetUp();
        float alignment = glm::dot(contactNormal, bounceUp);

        if (std::abs(alignment) > 0.7f) {
            float baseSpeed = sqrt(2.0f * BaseBounceHeight * -NFSEngine::PhysicsSystem::Gravity.y);
            float maxSpeed = sqrt(2.0f * MaxBounceHeight * -NFSEngine::PhysicsSystem::Gravity.y);

            float impactSpeed = std::abs(glm::dot(rb->PreviousVelocity, bounceUp));

            impactSpeed /= 1.414f;

            float kineticSpeed = impactSpeed * BounceMultiplier;

            float finalSpeed = std::clamp(std::max(baseSpeed, kineticSpeed), baseSpeed, maxSpeed);

            rb->Velocity = bounceUp * finalSpeed;

            rb->IsGrounded = false;
            rb->IsTouchingWall = false;

            targetObject->GetTransform()->Move(bounceUp * 0.1f);
        }
    }
};