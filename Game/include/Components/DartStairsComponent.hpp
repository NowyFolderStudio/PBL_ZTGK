#pragma once

#include <NFSEngine.h>
#include <vector>

struct SliceAnimationData {
    NFSEngine::GameObject* TargetObject;
    glm::vec3 StartPosition;
    glm::vec3 EndPosition;
};

class DartStairsComponent : public NFSEngine::Component {
public:
    DartStairsComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartStairsComponent"; }

    std::vector<NFSEngine::GameObject*> AllSlices;

    float TransitionDuration = 2.0f;

    void StartTransition() {
        if (AllSlices.empty()) return;

        m_Animations.clear();
        m_Timer = 0.0f;
        m_IsAnimating = true;
        
        NFSEngine::GameObject* selectedSlice = AllSlices[0];

        SliceAnimationData anim;
        anim.TargetObject = selectedSlice;
        anim.StartPosition = selectedSlice->GetTransform()->GetPosition();

        anim.EndPosition = anim.StartPosition + glm::vec3(0.0f, 10.0f, 0.0f); 
        
        m_Animations.push_back(anim);
        
        NFS_CORE_INFO("DartStairsComponent: Rozpoczynam fazę schodów!");
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_IsAnimating) return;

        m_Timer += deltaTime.GetSeconds();
        float progress = m_Timer / TransitionDuration;

        
        if (progress >= 1.0f) {
            progress = 1.0f;
            m_IsAnimating = false;
        }

        
        float easedProgress = progress * progress * (3.0f - 2.0f * progress);

        for (auto& anim : m_Animations) {
            glm::vec3 currentPos = glm::mix(anim.StartPosition, anim.EndPosition, easedProgress);
            anim.TargetObject->GetTransform()->SetPosition(currentPos);
        }
    }

private:
    bool m_IsAnimating = false;
    float m_Timer = 0.0f;
    std::vector<SliceAnimationData> m_Animations;
};