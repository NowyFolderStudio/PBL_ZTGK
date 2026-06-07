#pragma once
#include "Components/Component.hpp"
#include "Core/DeltaTime.hpp"
#include "Renderer/Animation.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include "imgui.h"
namespace NFSEngine {

    class AnimatorComponent : public Component {
    public:
        AnimatorComponent(GameObject* owner);

        virtual std::string GetName() const override { return "AnimatorComponent"; }
        void AddAnimation(const std::shared_ptr<Animation>& animation);
        void PlayAnimation(int index, bool looped = true);
        void OnUpdate(DeltaTime deltaTime) override;
        void SetAnimationSpeed(float speed) { m_AnimationSpeed = speed; }

        void OnImGuiRender() override { ImGui::DragFloat("Time", &m_CurrentTime, 0.1f); }

        std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
        float GetCurrentTime() const { return m_CurrentTime; }
        float GetAnimationLength() const { return m_Animations[m_CurrentAnimationIndex]->GetDuration(); }
        bool IsAnimationFinished() const { return m_Animations[m_CurrentAnimationIndex]->GetDuration() <= m_CurrentTime; }

    private:
        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

        std::vector<glm::mat4> m_FinalBoneMatrices;
        std::vector<std::shared_ptr<Animation>> m_Animations;
        int m_CurrentAnimationIndex = 0;
        float m_AnimationSpeed = 1.0f;
        float m_CurrentTime;
        float m_DeltaTime;
        bool m_Looped = true;
    };
} // namespace NFSEngine