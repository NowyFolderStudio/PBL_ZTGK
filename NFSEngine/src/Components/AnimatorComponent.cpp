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

    void AnimatorComponent::PlayAnimation(const std::shared_ptr<Animation>& animation) {
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0f;
    }

    void AnimatorComponent::OnUpdate(DeltaTime deltaTime) {
        m_DeltaTime = deltaTime;
        if (m_CurrentAnimation) {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone) {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

} // namespace NFSEngine