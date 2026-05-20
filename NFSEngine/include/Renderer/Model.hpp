#pragma once

#include "Renderer/Frustum.hpp"
#include "Renderer/VertexArray.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

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
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
    };

    class Model {
    public:
        Model(const std::string& path);

        const std::vector<MeshData>& GetMeshes() const { return m_Meshes; }

        const std::vector<AssimpMaterialInfo>& GetMaterialInfo() const { return m_MaterialInfo; }

        const BoundingSphere& GetMeshBoundingSphere() const { return m_MeshBoundingSphere; }
        const glm::vec3& GetMeshAABBMin() const { return m_MeshAABBMin; }
        const glm::vec3& GetMeshAABBMax() const { return m_MeshAABBMax; }

    private:
        std::vector<MeshData> m_Meshes;
        std::vector<AssimpMaterialInfo> m_MaterialInfo;

        std::string m_Directory;

        glm::vec3 m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());
        BoundingSphere m_MeshBoundingSphere;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        void FinalizeBoundingSphere();

        MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);
        void LoadMaterials(const aiScene* scene);
    };
} // namespace NFSEngine