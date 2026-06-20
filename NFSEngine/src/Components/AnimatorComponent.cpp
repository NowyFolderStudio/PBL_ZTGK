#include "Components/AnimatorComponent.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Core/Log.hpp"
#include <glm/ext.hpp>

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

    void AnimatorComponent::AddAnimation(const std::shared_ptr<Animation>& animation) { m_Animations.push_back(animation); }

    void AnimatorComponent::PlayAnimation(int index, bool looped) {
        if (index == m_CurrentAnimationIndex) return;

        if (index >= 0 && index < m_Animations.size()) {
            m_CurrentAnimationIndex = index;
        } else {
            m_CurrentAnimationIndex = 0;
        }

        m_CurrentTime = 0.0f;
        m_Looped = looped;

        m_PreviousAnimationIndex = -1;
        m_PreviousTime = 0.0f;
        m_BlendFactor = 1.0f;
    }

    void AnimatorComponent::PlayAnimationBlended(int index, float blendDuration, bool looped) {
        if (index == m_CurrentAnimationIndex) return;

        if (index >= 0 && index < m_Animations.size()) {
            if (!m_Animations.empty() && m_Animations[m_CurrentAnimationIndex]) {
                m_PreviousAnimationIndex = m_CurrentAnimationIndex;
                m_PreviousTime = m_CurrentTime;
                m_BlendFactor = 0.0f;
                m_BlendDuration = blendDuration > 0.0f ? blendDuration : 0.001f;
            } else {
                m_BlendFactor = 1.0f;
                m_PreviousAnimationIndex = -1;
            }

            m_CurrentAnimationIndex = index;
            m_CurrentTime = 0.0f;
        } else {
            m_CurrentAnimationIndex = 0;
            m_BlendFactor = 1.0f;
            m_PreviousAnimationIndex = -1;
        }
        m_Looped = looped;
    }

    void AnimatorComponent::OnUpdate(DeltaTime deltaTime) {
        m_DeltaTime = deltaTime;
        if (m_Animations.empty() || !m_Animations[m_CurrentAnimationIndex]) return;

        auto animation = m_Animations[m_CurrentAnimationIndex];
        m_CurrentTime += animation->GetTicksPerSecond() * deltaTime * m_AnimationSpeed;
        if (m_Looped)
            m_CurrentTime = fmod(m_CurrentTime, animation->GetDuration());
        else
            m_CurrentTime = glm::min(m_CurrentTime, GetAnimationLength());

        if (m_PreviousAnimationIndex != -1 && m_BlendFactor < 1.0f) {
            auto prevAnimation = m_Animations[m_PreviousAnimationIndex];
            m_PreviousTime += prevAnimation->GetTicksPerSecond() * deltaTime * m_AnimationSpeed;
            m_PreviousTime = fmod(m_PreviousTime, prevAnimation->GetDuration());

            float blendSpeed = 1.0f / m_BlendDuration;
            m_BlendFactor += deltaTime * blendSpeed;

            if (m_BlendFactor >= 1.0f) {
                m_BlendFactor = 1.0f;
                m_PreviousAnimationIndex = -1;
            }
        }

        CalculateBoneTransform(&animation->GetRootNode(), glm::mat4(1.0f));
    }

    void AnimatorComponent::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        auto animation = m_Animations[m_CurrentAnimationIndex];
        Bone* currentBone = animation->FindBone(nodeName);

        bool isBlending = (m_PreviousAnimationIndex != -1 && m_BlendFactor < 1.0f);

        if (isBlending) {
            auto prevAnimation = m_Animations[m_PreviousAnimationIndex];
            Bone* prevBone = prevAnimation->FindBone(nodeName);

            glm::vec3 prevPos = prevBone ? prevBone->GetLocalPosition(m_PreviousTime) : glm::vec3(0.0f);
            glm::quat prevRot = prevBone ? prevBone->GetLocalRotation(m_PreviousTime) : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 prevScale = prevBone ? prevBone->GetLocalScale(m_PreviousTime) : glm::vec3(1.0f);

            glm::vec3 currPos = currentBone ? currentBone->GetLocalPosition(m_CurrentTime) : glm::vec3(0.0f);
            glm::quat currRot = currentBone ? currentBone->GetLocalRotation(m_CurrentTime) : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 currScale = currentBone ? currentBone->GetLocalScale(m_CurrentTime) : glm::vec3(1.0f);

            if (currentBone || prevBone) {
                glm::vec3 blendedPos = glm::mix(prevPos, currPos, m_BlendFactor);
                glm::quat blendedRot = glm::slerp(prevRot, currRot, m_BlendFactor);
                glm::vec3 blendedScale = glm::mix(prevScale, currScale, m_BlendFactor);

                glm::mat4 translationM = glm::translate(glm::mat4(1.0f), blendedPos);
                glm::mat4 rotationM = glm::toMat4(blendedRot);
                glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), blendedScale);

                nodeTransform = translationM * rotationM * scaleM;
            }
        } else {
            if (currentBone) {
                currentBone->Update(m_CurrentTime);
                nodeTransform = currentBone->GetLocalTransform();
            }
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;

            if (index >= m_FinalBoneMatrices.size()) {
                NFS_ERROR("Invalid index of bone: {}", index);
                assert(false);
            }
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

} // namespace NFSEngine