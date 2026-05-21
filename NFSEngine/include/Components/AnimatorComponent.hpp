#pragma once
#include "Components/Component.hpp"
#include "Core/DeltaTime.hpp"
#include "Renderer/Animation.hpp"
#include "Renderer/Model.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

namespace NFSEngine {

    class AnimatorComponent : public Component {
    public:
        AnimatorComponent(GameObject* owner);

        virtual std::string GetName() const override { return "AnimatorComponent"; }
        void PlayAnimation(std::shared_ptr<Animation> animation);
        void OnAwake() override;
        void OnUpdate(DeltaTime deltaTime) override;

        const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

    private:
        void CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform);
        int GetPositionIndex(float animationTime, const BoneTransformTrack& track);
        int GetRotationIndex(float animationTime, const BoneTransformTrack& track);
        int GetScaleIndex(float animationTime, const BoneTransformTrack& track);

        glm::vec3 InterpolatePosition(float animationTime, const BoneTransformTrack& track);
        glm::quat InterpolateRotation(float animationTime, const BoneTransformTrack& track);
        glm::vec3 InterpolateScale(float animationTime, const BoneTransformTrack& track);

        std::vector<glm::mat4> m_FinalBoneMatrices;
        std::shared_ptr<Animation> m_CurrentAnimation;
        Model* m_CurrentModel;
        float m_CurrentTime;
    };
} // namespace NFSEngine