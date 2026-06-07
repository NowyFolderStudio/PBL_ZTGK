#include "NFSEngine.h"

class CharacterAnimationController : public NFSEngine::Component {
public:
    CharacterAnimationController(NFSEngine::GameObject* owner)
        : Component(owner) { }
    std::string GetName() const override { return "CharacterAnimationController"; }

protected:
    void OnStart() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void UpdateStates();
    void ChangeAnimation();

private:
    bool m_OnGround = false;
    bool m_InMotion = false;
    float m_VerticalSpeed = 0.0f;
    float m_HorizontalSpeed = 0.0f;
    float m_MaxSpeed = 0.0f;
    NFSEngine::RigidBody3DComponent* m_Rigidbody;
    NFSEngine::AnimatorComponent* m_Animator;
};