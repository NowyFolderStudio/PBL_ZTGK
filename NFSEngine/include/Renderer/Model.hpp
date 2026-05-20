#pragma once

#include "Renderer/Frustum.hpp"
#include "Renderer/VertexArray.hpp"
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <map>

#define MAX_BONE_INFLUENCE 4

namespace NFSEngine {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

        std::array<int, MAX_BONE_INFLUENCE> boneIDs = { -1, -1, -1, -1 };
        std::array<float, MAX_BONE_INFLUENCE> boneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    class Model {
    public:
        Model(const std::string& path);

        std::vector<std::shared_ptr<VertexArray>> GetMeshes() const { return m_Meshes; }

        const BoundingSphere& GetMeshBoundingSphere() const { return m_MeshBoundingSphere; }
        const glm::vec3& GetMeshAABBMin() const { return m_MeshAABBMin; }
        const glm::vec3& GetMeshAABBMax() const { return m_MeshAABBMax; }

        std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
        int GetBoneCount() const { return m_BoneCounter; }

    private:
        std::vector<std::shared_ptr<VertexArray>> m_Meshes;

        glm::vec3 m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());
        BoundingSphere m_MeshBoundingSphere;

        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        void FinalizeBoundingSphere();

        static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

        std::shared_ptr<VertexArray> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
} // namespace NFSEngine