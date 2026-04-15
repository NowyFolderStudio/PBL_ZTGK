#include "Renderer/Model.hpp"
#include <iostream>

namespace NFSEngine {
    Model::Model(const std::string& path) { LoadModel(path); }

    void Model::LoadModel(const std::string& path) {
        Assimp::Importer import;

        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    std::shared_ptr<VertexArray> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            if (mesh->HasNormals()) {
                vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            } else {
                vertex.Normal = glm::vec3(0.0f);
            }

            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            } else {
                vertex.TexCoords = glm::vec2(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        std::shared_ptr<VertexArray> vao = std::shared_ptr<VertexArray>(VertexArray::Create());

        std::shared_ptr<VertexBuffer> vbo
            = std::shared_ptr<VertexBuffer>(VertexBuffer::Create((float*)vertices.data(), vertices.size() * sizeof(Vertex)));

        BufferLayout layout = { { ShaderDataType::Float3, "a_Position" },
                                { ShaderDataType::Float3, "a_Normal" },
                                { ShaderDataType::Float2, "a_TexCoord" } };
        vbo->SetLayout(layout);

        vao->AddVertexBuffer(vbo);

        std::shared_ptr<IndexBuffer> ibo = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices.data(), indices.size()));

        vao->SetIndexBuffer(ibo);

        return vao;
    }
} // namespace NFSEngine
