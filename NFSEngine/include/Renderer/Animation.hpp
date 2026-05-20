#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "nfspch.h"

namespace NFSEngine {

    class Model;

    struct AssimpNodeData {
        glm::mat4 transformation;
        std::string name;
        std::vector<AssimpNodeData> children;
    };

    template <typename T> struct KeyFrame {
        float timeStamp;
        T value;
    };

    struct BoneTransformTrack {
        std::vector<KeyFrame<glm::vec3>> positions;
        std::vector<KeyFrame<glm::quat>> rotations;
        std::vector<KeyFrame<glm::vec3>> scales;
    };

    class Animation {
    public:
        static std::shared_ptr<Animation> LoadSingle(const std::string& path, Model* model);
        static std::shared_ptr<Animation> LoadByName(const std::string& path, Model* model, const std::string& animationName);
        static std::shared_ptr<Animation> LoadSliced(const std::string& path, Model* model, float startTime, float endTime);
        static std::vector<std::shared_ptr<Animation>> LoadAll(const std::string& path, Model* model);

        float GetTicksPerSecond() const { return m_TicksPerSecond; }
        float GetDuration() const { return m_Duration; }
        const AssimpNodeData& GetRootNode() const { return m_RootNode; }
        const std::map<std::string, BoneTransformTrack>& GetBones() const { return m_Bones; }
        const std::string& GetName() const { return m_Name; }

    private:
        Animation() = default;
        void SetupFromAssimp(const aiAnimation* animation, const aiScene* scene, Model* model);
        void SetupSlicedFromAssimp(const aiAnimation* animation, const aiScene* scene, Model* model, float start, float end);

        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
        void ReadMissingBones(const aiAnimation* animation, Model* model);
        void ReadSlicedBones(const aiAnimation* animation, Model* model, float start, float end);

        static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
        static glm::vec3 GetGLMVec(const aiVector3D& vec);
        static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);

        float m_Duration = 0.0f;
        int m_TicksPerSecond = 0;
        std::map<std::string, BoneTransformTrack> m_Bones;
        AssimpNodeData m_RootNode;
        std::string m_Name;
    };
} // namespace NFSEngine