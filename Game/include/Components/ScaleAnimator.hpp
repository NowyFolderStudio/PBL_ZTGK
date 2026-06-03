#pragma once

#include <NFSEngine.h>

class ScaleAnimator : public NFSEngine::Component {
public:
    ScaleAnimator(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "ScaleAnimator"; }

    float AnimationSpeed = 10.0f;

    void SetTargetScale(const glm::vec3& target) { m_TargetScale = target; }

    void SetTargetScaleY(float targetY) { m_TargetScale.y = targetY; }

protected:
    glm::vec3 m_TargetScale = glm::vec3(1.0f);

    void OnAwake() override { }

    void OnStart() override {
        if (GetOwner() && GetOwner()->GetTransform()) {
            m_TargetScale = GetOwner()->GetTransform()->GetScale();
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        auto* transform = GetOwner()->GetTransform();
        if (!transform) return;

        glm::vec3 currentScale = transform->GetScale();

        float dt = static_cast<float>(deltaTime);
        glm::vec3 newScale = glm::mix(currentScale, m_TargetScale, dt * AnimationSpeed);

        if (glm::distance(newScale, m_TargetScale) > 0.001f) {
            transform->SetScale(newScale);
        } else {
            transform->SetScale(m_TargetScale);
        }
    }

    void OnEnable() override { }

    void OnDisable() override { }
};
