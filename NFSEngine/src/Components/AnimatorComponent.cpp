#include "Components/AnimatorComponent.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
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
        if (index != m_CurrentAnimationIndex) {
            m_CurrentTime = 0.0f;
        }
        if (index >= 0 && index < m_Animations.size()) {
            m_CurrentAnimationIndex = index;
        } else {
            m_CurrentAnimationIndex = 0;
        }
        m_Looped = looped;
    }

    void AnimatorComponent::OnUpdate(DeltaTime deltaTime) {
        m_DeltaTime = deltaTime;
        if (!m_Animations.empty() && m_Animations[m_CurrentAnimationIndex]) {
            auto animation = m_Animations[m_CurrentAnimationIndex];
            m_CurrentTime += animation->GetTicksPerSecond() * deltaTime * m_AnimationSpeed;
            if (m_Looped)
                m_CurrentTime = fmod(m_CurrentTime, animation->GetDuration());
            else
                m_CurrentTime = glm::min(m_CurrentTime, GetAnimationLength());
            CalculateBoneTransform(&animation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;
        auto animation = m_Animations[m_CurrentAnimationIndex];

        Bone* Bone = animation->FindBone(nodeName);

        if (Bone) {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

} // namespace NFSEngine