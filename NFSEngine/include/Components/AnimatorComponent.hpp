#pragma once
#include "Components/Component.hpp"
#include "Core/DeltaTime.hpp"
#include "Renderer/Animation.hpp"
#include "Renderer/Model.hpp"
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
        void PlayAnimation(const std::shared_ptr<Animation>& animation);
        void OnUpdate(DeltaTime deltaTime) override;

        void OnImGuiRender() override { ImGui::DragFloat("Time", &m_CurrentTime, 0.1f); }

        std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; };

    private:
        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

        std::vector<glm::mat4> m_FinalBoneMatrices;
        std::shared_ptr<Animation> m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
    };
} // namespace NFSEngine