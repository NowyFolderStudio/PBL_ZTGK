#include "Components/AnimatorComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <cassert>

namespace NFSEngine {

    AnimatorComponent::AnimatorComponent(GameObject* owner)
        : Component(owner) {
        m_CurrentTime = 0.0f;
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
    }

    void AnimatorComponent::OnAwake() {
        if (!m_Owner->GetComponent<ModelComponent>()) return;
        auto LODs = m_Owner->GetComponent<ModelComponent>()->GetLODs();
        if (LODs.empty()) return;
        m_CurrentModel = m_Owner->GetComponent<ModelComponent>()->GetLODs()[0].ModelData.get();
    }

    void AnimatorComponent::PlayAnimation(std::shared_ptr<Animation> animation) {
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0f;
    }

    void AnimatorComponent::OnUpdate(DeltaTime deltaTime) {
        if (!m_CurrentAnimation || !m_CurrentModel) return;
        float ticksPerSecond = m_CurrentAnimation->GetTicksPerSecond();
        if (ticksPerSecond == 0.0f) ticksPerSecond = 25.0f; // Domyślnie 25 FPS

        m_CurrentTime += deltaTime * ticksPerSecond;
        m_CurrentTime = std::fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }

    void AnimatorComponent::CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        const auto& boneTracks = m_CurrentAnimation->GetBones();
        if (boneTracks.find(nodeName) != boneTracks.end()) {
            const auto& track = boneTracks.at(nodeName);

            glm::vec3 defaultPos = glm::vec3(nodeTransform[3]);

            glm::vec3 defaultScale = glm::vec3(glm::length(glm::vec3(nodeTransform[0])), glm::length(glm::vec3(nodeTransform[1])),
                                               glm::length(glm::vec3(nodeTransform[2])));

            glm::vec3 pos = track.positions.empty() ? defaultPos : InterpolatePosition(m_CurrentTime, track);
            glm::quat rot = InterpolateRotation(m_CurrentTime, track);
            glm::vec3 scale = track.scales.empty() ? defaultScale : InterpolateScale(m_CurrentTime, track);

            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), pos);
            glm::mat4 rotationMat = glm::mat4_cast(rot);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            nodeTransform = translationMat * rotationMat * scaleMat;
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        for (const auto& child : node->children) {
            CalculateBoneTransform(&child, globalTransform);
        }
    }

    int AnimatorComponent::GetPositionIndex(float animationTime, const BoneTransformTrack& track) {
        if (track.positions.size() < 2) return 0;
        for (int i = 0; i < track.positions.size() - 1; ++i) {
            if (animationTime < track.positions[i + 1].timeStamp) return i;
        }
        return track.positions.size() - 2;
    }

    int AnimatorComponent::GetRotationIndex(float animationTime, const BoneTransformTrack& track) {
        if (track.rotations.size() < 2) return 0;
        for (int i = 0; i < track.rotations.size() - 1; ++i) {
            if (animationTime < track.rotations[i + 1].timeStamp) return i;
        }
        return track.rotations.size() - 2;
    }

    int AnimatorComponent::GetScaleIndex(float animationTime, const BoneTransformTrack& track) {
        if (track.scales.size() < 2) return 0;
        for (int i = 0; i < track.scales.size() - 1; ++i) {
            if (animationTime < track.scales[i + 1].timeStamp) return i;
        }
        return track.scales.size() - 2;
    }

    glm::vec3 AnimatorComponent::InterpolatePosition(float animationTime, const BoneTransformTrack& track) {
        if (track.positions.size() == 1) return track.positions[0].value;

        int p0Index = GetPositionIndex(animationTime, track);
        int p1Index = p0Index + 1;

        float t1 = track.positions[p0Index].timeStamp;
        float t2 = track.positions[p1Index].timeStamp;
        float deltaTime = t2 - t1;

        float scaleFactor = (deltaTime > 0.0001f) ? (animationTime - t1) / deltaTime : 0.0f;

        scaleFactor = glm::clamp(scaleFactor, 0.0f, 1.0f);

        return glm::mix(track.positions[p0Index].value, track.positions[p1Index].value, scaleFactor);
    }

    glm::quat AnimatorComponent::InterpolateRotation(float animationTime, const BoneTransformTrack& track) {
        if (track.rotations.size() == 1) return glm::normalize(track.rotations[0].value);

        int p0Index = GetRotationIndex(animationTime, track);
        int p1Index = p0Index + 1;

        float t1 = track.rotations[p0Index].timeStamp;
        float t2 = track.rotations[p1Index].timeStamp;
        float deltaTime = t2 - t1;

        float scaleFactor = (deltaTime > 0.0001f) ? (animationTime - t1) / deltaTime : 0.0f;
        scaleFactor = glm::clamp(scaleFactor, 0.0f, 1.0f);

        glm::quat q1 = track.rotations[p0Index].value;
        glm::quat q2 = track.rotations[p1Index].value;

        if (glm::dot(q1, q2) < 0.0f) {
            q2 = -q2;
        }

        glm::quat finalRot = glm::slerp(q1, q2, scaleFactor);

        if (glm::length(finalRot) < 0.00001f) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        return glm::normalize(finalRot);
    }

    glm::vec3 AnimatorComponent::InterpolateScale(float animationTime, const BoneTransformTrack& track) {
        if (track.scales.size() == 1) return track.scales[0].value;

        int p0Index = GetScaleIndex(animationTime, track);
        int p1Index = p0Index + 1;

        float t1 = track.scales[p0Index].timeStamp;
        float t2 = track.scales[p1Index].timeStamp;
        float deltaTime = t2 - t1;
        float scaleFactor = (deltaTime > 0.0001f) ? (animationTime - t1) / deltaTime : 0.0f;

        scaleFactor = glm::clamp(scaleFactor, 0.0f, 1.0f);

        return glm::mix(track.scales[p0Index].value, track.scales[p1Index].value, scaleFactor);
    }

} // namespace NFSEngine