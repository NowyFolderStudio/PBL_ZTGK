#pragma once

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

    private:
        std::vector<std::shared_ptr<VertexArray>> m_Meshes;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);

        std::shared_ptr<VertexArray> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
} // namespace NFSEngine