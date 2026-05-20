#include "Renderer/Model.hpp"
#include <iostream>
#include <limits>

namespace NFSEngine {
    Model::Model(const std::string& path) {
        m_MeshAABBMin = glm::vec3(std::numeric_limits<float>::max());
        m_MeshAABBMax = glm::vec3(std::numeric_limits<float>::lowest());

        m_Directory = std::filesystem::path(path).parent_path().string();
        LoadModel(path);
        FinalizeBoundingSphere();
    }

    void Model::LoadModel(const std::string& path) {
        Assimp::Importer import;

        const aiScene* scene
            = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        LoadMaterials(scene);
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::LoadMaterials(const aiScene* scene) {
        m_MaterialInfo.reserve(scene->mNumMaterials);

        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* material = scene->mMaterials[i];
            AssimpMaterialInfo info;

            aiString name;
            material->Get(AI_MATKEY_NAME, name);
            info.Name = name.C_Str();

            auto extractPath = [&](aiTextureType type, std::string& outPath) {
                if (material->GetTextureCount(type) > 0) {
                    aiString path;
                    material->GetTexture(type, 0, &path);
                    outPath = m_Directory + "/" + path.C_Str();
                }
            };

            extractPath(aiTextureType_DIFFUSE, info.AlbedoPath);
            extractPath(aiTextureType_NORMALS, info.NormalPath);
            extractPath(aiTextureType_DIFFUSE_ROUGHNESS, info.RoughnessPath);
            extractPath(aiTextureType_METALNESS, info.MetallicPath);
            extractPath(aiTextureType_AMBIENT_OCCLUSION, info.AmbientOcclusionPath);

            m_MaterialInfo.push_back(info);
        }
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

    MeshData Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            m_MeshAABBMin = glm::min(m_MeshAABBMin, vertex.position);
            m_MeshAABBMax = glm::max(m_MeshAABBMax, vertex.position);

            if (mesh->HasNormals()) {
                vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            } else {
                vertex.normal = glm::vec3(0.0f);
            }

            if (mesh->mTextureCoords[0]) {
                vertex.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            } else {
                vertex.texCoords = glm::vec2(0.0f);
            }

            if (mesh->HasTangentsAndBitangents()) {
                vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            } else {
                vertex.tangent = glm::vec3(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        std::shared_ptr<VertexArray> vao = std::shared_ptr<VertexArray>(VertexArray::Create());

        std::shared_ptr<VertexBuffer> vbo
            = std::shared_ptr<VertexBuffer>(VertexBuffer::Create((float*)vertices.data(), vertices.size() * sizeof(Vertex)));

        BufferLayout layout = { { ShaderDataType::Float3, "a_Position" }, { ShaderDataType::Float3, "a_Normal" },
                                { ShaderDataType::Float2, "a_TexCoord" }, { ShaderDataType::Float3, "a_Tangent" },
                                { ShaderDataType::Int4, "a_BoneIDs" },    { ShaderDataType::Float4, "a_Weights" } };

        vbo->SetLayout(layout);

        vao->AddVertexBuffer(vbo);

        std::shared_ptr<IndexBuffer> ibo = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices.data(), indices.size()));

        vao->SetIndexBuffer(ibo);

        MeshData meshData;
        meshData.VAO = vao;
        meshData.MaterialIndex = mesh->mMaterialIndex; // Kluczowy moment!

        return meshData;
    }

    glm::mat4 Model::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
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

    void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
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

                for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                    if (vertices[vertexId].boneIDs[i] < 0) {
                        vertices[vertexId].boneWeights[i] = weight;
                        vertices[vertexId].boneIDs[i] = boneID;
                        break;
                    }
                }
            }
        }
    }
} // namespace NFSEngine
