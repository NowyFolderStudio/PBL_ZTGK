#pragma once

#include "Renderer/Frustum.hpp"
#include "Renderer/VertexArray.hpp"
#include <array>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <map>

#define MAX_NUM_BONES_PER_VERTEX 4

namespace NFSEngine {

    struct MeshData {
        std::shared_ptr<VertexArray> VAO;
        unsigned int MaterialIndex = 0;
    };

    struct AssimpMaterialInfo {
        std::string Name;
        std::string AlbedoPath;
        std::string NormalPath;
        std::string RoughnessPath;
        std::string MetallicPath;
        std::string AmbientOcclusionPath;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;

        std::array<int, MAX_NUM_BONES_PER_VERTEX> boneIDs = { -1, -1, -1, -1 };
        std::array<float, MAX_NUM_BONES_PER_VERTEX> weights = { 0.0f, 0.0f, 0.0f, 0.0f };

        void AddBoneData(int boneID, float weight) {
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
                if (weights[i] == 0.0f) {
                    boneIDs[i] = boneID;
                    weights[i] = weight;
                    return;
                }
            }
        }
    };

    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    class Model {
    public:
        Model(const std::string& path);

        const std::vector<MeshData>& GetMeshes() const { return m_Meshes; }


        const std::vector<AssimpMaterialInfo>& GetMaterialInfo() const { return m_MaterialInfo; }

        const BoundingSphere& GetMeshBoundingSphere() const { return m_MeshBoundingSphere; }
        const glm::vec3& GetMeshAABBMin() const { return m_MeshAABBMin; }
        const glm::vec3& GetMeshAABBMax() const { return m_MeshAABBMax; }

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

    private:
        std::vector<MeshData> m_Meshes;
        std::vector<AssimpMaterialInfo> m_MaterialInfo;

        // BONES DATA
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        std::string m_Directory;

        glm::vec3 m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());
        BoundingSphere m_MeshBoundingSphere;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);

        void FinalizeBoundingSphere();

        void LoadMaterials(const aiScene* scene);

        void SetVertexBoneDataToDefault(Vertex& vertex) {
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
                vertex.boneIDs[i] = -1;
                vertex.weights[i] = 0.0f;
            }
        }

        void SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i) {
                if (vertex.boneIDs[i] < 0) {
                    vertex.weights[i] = weight;
                    vertex.boneIDs[i] = boneID;
                    break;
                }
            }
        }

        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                int boneID = -1;
                std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                    BoneInfo newBoneInfo;
                    newBoneInfo.id = m_BoneCounter;
                    newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                    m_BoneInfoMap[boneName] = newBoneInfo;
                    boneID = m_BoneCounter;
                    m_BoneCounter++;
                } else {
                    boneID = m_BoneInfoMap[boneName].id;
                }
                assert(boneID != -1);
                auto weights = mesh->mBones[boneIndex]->mWeights;
                int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                    int vertexId = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;
                    assert(vertexId <= vertices.size());
                    SetVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }

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