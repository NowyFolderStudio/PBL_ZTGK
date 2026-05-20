#pragma once
#include <NFSEngine.h>

class BounceComponent : public NFSEngine::Component {
public:
    BounceComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "BounceComponent"; }

    float BounceHeight = 15.0f;

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
    }

private:
    void ApplyBounce(NFSEngine::GameObject* targetObject, glm::vec3 contactNormal) {
        auto* rb = targetObject->GetComponent<NFSEngine::RigidBody3DComponent>();
        if (!rb) return;

        glm::vec3 bounceUp = m_Owner->GetTransform()->GetUp();
        float alignment = glm::dot(contactNormal, bounceUp);

        if (std::abs(alignment) > 0.7f) {
            float speed = sqrt(2.0f * BounceHeight * -NFSEngine::PhysicsSystem::Gravity.y);

            rb->Velocity = bounceUp * speed;
            rb->IsGrounded = false;
            rb->IsTouchingWall = false;

            targetObject->GetTransform()->Move(bounceUp * 0.1f);
        }
    }
};