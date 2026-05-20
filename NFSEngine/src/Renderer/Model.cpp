#include "Renderer/Model.hpp"
#include <iostream>
#include <limits>

namespace NFSEngine {
    Model::Model(const std::string& path) {
        m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());
        LoadModel(path);
        FinalizeBoundingSphere();
    }

    void Model::LoadModel(const std::string& path) {
        Assimp::Importer import;

        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

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

    void Model::FinalizeBoundingSphere() {
        if (m_MeshAABBMin.x > m_MeshAABBMax.x) {
            m_MeshBoundingSphere = { glm::vec3(0.0f), 0.5f };
            return;
        }
        glm::vec3 center = (m_MeshAABBMin + m_MeshAABBMax) * 0.5f;
        float radius = glm::length(m_MeshAABBMax - center);
        if (radius < 0.001f) radius = 0.5f;
        m_MeshBoundingSphere = { center, radius };
    }

    std::shared_ptr<VertexArray> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            m_MeshAABBMin = glm::min(m_MeshAABBMin, vertex.Position);
            m_MeshAABBMax = glm::max(m_MeshAABBMax, vertex.Position);

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

            if (mesh->HasTangentsAndBitangents()) {
                vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            }
            else {
                vertex.Tangent = glm::vec3(0.0f);
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
                                { ShaderDataType::Float2, "a_TexCoord" },
                                { ShaderDataType::Float3, "a_Tangent" } 
        };
        vbo->SetLayout(layout);

        vao->AddVertexBuffer(vbo);

        std::shared_ptr<IndexBuffer> ibo = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices.data(), indices.size()));

        vao->SetIndexBuffer(ibo);

        return vao;
    }
} // namespace NFSEngine
