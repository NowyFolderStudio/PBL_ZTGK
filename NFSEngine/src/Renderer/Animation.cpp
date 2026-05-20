#include "Renderer/Animation.hpp"
#include "Core/Log.hpp"
#include "Renderer/Model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace NFSEngine {
    std::shared_ptr<Animation> Animation::LoadSingle(const std::string& path, Model* model) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode || scene->mNumAnimations == 0) {
            NFS_CORE_ERROR("ERROR::ANIMATION::Animation not found in file: {}", path);
            return nullptr;
        }

        auto anim = std::shared_ptr<Animation>(new Animation());
        anim->SetupFromAssimp(scene->mAnimations[0], scene, model);
        return anim;
    }

    std::shared_ptr<Animation> Animation::LoadByName(const std::string& path, Model* model, const std::string& animationName) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode || scene->mNumAnimations == 0) {
            return nullptr;
        }

        aiAnimation* targetAnim = nullptr;
        for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
            std::string currentName = scene->mAnimations[i]->mName.C_Str();
            if (currentName.find(animationName) != std::string::npos) {
                targetAnim = scene->mAnimations[i];
                break;
            }
        }

        if (!targetAnim) {
            NFS_CORE_ERROR("ERROR::ANIMATION::Animation with name {} not found in file: {}", animationName, path);
            return nullptr;
        }

        auto anim = std::shared_ptr<Animation>(new Animation());
        anim->SetupFromAssimp(targetAnim, scene, model);
        return anim;
    }

    std::shared_ptr<Animation> Animation::LoadSliced(const std::string& path, Model* model, float startTime, float endTime) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode || scene->mNumAnimations == 0) {
            return nullptr;
        }

        auto anim = std::shared_ptr<Animation>(new Animation());
        anim->SetupSlicedFromAssimp(scene->mAnimations[0], scene, model, startTime, endTime);
        return anim;
    }

    std::vector<std::shared_ptr<Animation>> Animation::LoadAll(const std::string& path, Model* model) {
        std::vector<std::shared_ptr<Animation>> animations;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode || scene->mNumAnimations == 0) {
            NFS_CORE_ERROR("ERROR::ANIMATION::Animation not found in file: {}", path);
            return animations;
        }

        for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
            auto anim = std::shared_ptr<Animation>(new Animation());

            anim->m_Name = scene->mAnimations[i]->mName.C_Str();
            anim->SetupFromAssimp(scene->mAnimations[i], scene, model);

            animations.push_back(anim);
        }

        return animations;
    }

    void Animation::SetupSlicedFromAssimp(const aiAnimation* animation, const aiScene* scene, Model* model, float start,
                                          float end) {
        m_Duration = end - start;
        m_TicksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;

        ReadHierarchyData(m_RootNode, scene->mRootNode);

        ReadSlicedBones(animation, model, start, end);
    }

    void Animation::ReadSlicedBones(const aiAnimation* animation, Model* model, float start, float end) {
        int size = animation->mNumChannels;

        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            BoneTransformTrack track;

            for (unsigned int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex) {
                aiVectorKey key = channel->mPositionKeys[positionIndex];
                float time = (float)key.mTime;

                if (time >= start && time <= end) {
                    track.positions.push_back({ time - start, GetGLMVec(key.mValue) });
                }
            }

            for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex) {
                aiQuatKey key = channel->mRotationKeys[rotationIndex];
                float time = (float)key.mTime;

                if (time >= start && time <= end) {
                    track.rotations.push_back({ time - start, GetGLMQuat(key.mValue) });
                }
            }

            for (unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex) {
                aiVectorKey key = channel->mScalingKeys[scaleIndex];
                float time = (float)key.mTime;

                if (time >= start && time <= end) {
                    track.scales.push_back({ time - start, GetGLMVec(key.mValue) });
                }
            }

            m_Bones[boneName] = track;
        }
    }

    void Animation::SetupFromAssimp(const aiAnimation* animation, const aiScene* scene, Model* model) {
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;
        ReadHierarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, model);
    }

    void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);

        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    void Animation::ReadMissingBones(const aiAnimation* animation, Model* model) {
        int size = animation->mNumChannels;

        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            BoneTransformTrack track;

            for (unsigned int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex) {
                aiVectorKey key = channel->mPositionKeys[positionIndex];
                track.positions.push_back({ (float)key.mTime, GetGLMVec(key.mValue) });
            }

            for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex) {
                aiQuatKey key = channel->mRotationKeys[rotationIndex];
                track.rotations.push_back({ (float)key.mTime, GetGLMQuat(key.mValue) });
            }

            // 3. Wyciągamy klatki kluczowe SKALI
            for (unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex) {
                aiVectorKey key = channel->mScalingKeys[scaleIndex];
                track.scales.push_back({ (float)key.mTime, GetGLMVec(key.mValue) });
            }

            m_Bones[boneName] = track;
        }
    }

    glm::mat4 Animation::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
        glm::mat4 to;
        // Pamiętaj o transponowaniu! (Assimp to wiersze, GLM to kolumny)
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

    glm::vec3 Animation::GetGLMVec(const aiVector3D& vec) { return { vec.x, vec.y, vec.z }; }

    glm::quat Animation::GetGLMQuat(const aiQuaternion& pOrientation) {
        return { pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z };
    }

} // namespace NFSEngine