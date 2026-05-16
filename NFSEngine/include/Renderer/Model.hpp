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

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class Model {
    public:
        Model(const std::string& path);

        std::vector<std::shared_ptr<VertexArray>> GetMeshes() const { return m_Meshes; }

        const BoundingSphere& GetMeshBoundingSphere() const { return m_MeshBoundingSphere; }
        const glm::vec3& GetMeshAABBMin() const { return m_MeshAABBMin; }
        const glm::vec3& GetMeshAABBMax() const { return m_MeshAABBMax; }

    private:
        std::vector<std::shared_ptr<VertexArray>> m_Meshes;

        glm::vec3 m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());
        BoundingSphere m_MeshBoundingSphere;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        void FinalizeBoundingSphere();

        std::shared_ptr<VertexArray> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
} // namespace NFSEngine