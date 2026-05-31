#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Core/Log.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Bone.hpp"
#include "assimp/anim.h"
#include <assimp/postprocess.h>

namespace NFSEngine {

    struct AssimpNodeData {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation {
    public:
        Animation(const std::string& animationPath, Model* model) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }

        Animation(const std::string& animationPath, Model* model, int animationIndex) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            aiAnimation* animation = scene->mAnimations[0];

            if (animationIndex < scene->mNumAnimations) {
                animation = scene->mAnimations[animationIndex];
            } else {
                NFS_CORE_WARN("Animation indes {} do not exist in this file: {}", animationIndex, animationPath);
            }

            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            m_Name = animation->mName.C_Str();
            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }

        ~Animation() { }

        Bone* FindBone(const std::string& name) {
            auto iter
                = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& Bone) { return Bone.GetBoneName() == name; });
            if (iter == m_Bones.end())
                return nullptr;
            else
                return &(*iter);
        }

        float GetTicksPerSecond() { return m_TicksPerSecond; }
        float GetDuration() { return m_Duration; }
        std::string GetName() { return m_Name; }
        const AssimpNodeData& GetRootNode() { return m_RootNode; }
        const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

    private:
        void ReadMissingBones(const aiAnimation* animation, Model& model) {
            int size = animation->mNumChannels;

            auto& boneInfoMap = model.GetBoneInfoMap(); // getting m_BoneInfoMap from Model class
            int& boneCount = model.GetBoneCount(); // getting the m_BoneCounter from Model class

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++) {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++) {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        std::string m_Name;

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
            glm::mat4 to;
            to[0][0] = from.a1;
            to[1][0] = from.a2;
            to[2][0] = from.a3;
            to[3][0] = from.a4;
            to[0][1] = from.b1;
            to[1][1] = from.b2;
            to[2][1] = from.b3;
            to[3][1] = from.b4;
            to[0][2] = from.c1;
            to[1][2] = from.c2;
            to[2][2] = from.c3;
            to[3][2] = from.c4;
            to[0][3] = from.d1;
            to[1][3] = from.d2;
            to[2][3] = from.d3;
            to[3][3] = from.d4;
            return to;
        }
    };
} // namespace NFSEngine