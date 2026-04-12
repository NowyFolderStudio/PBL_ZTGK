#include "Components/CubeMesh.hpp"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine
{

CubeMesh::CubeMesh(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
    : Component(owner)
    , p_Shader(std::move(shader))
    , p_Texture(std::move(texture))
{

    p_VertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());

    float vertices[] = {
        // Pozycje (x, y, z)    // Tekstury (u, v)
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

         0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f
    };

    uint32_t indices[] = {
         0,  1,  2,  2,  3,  0,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    std::shared_ptr<VertexBuffer> vbo;
    vbo = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));

    BufferLayout layout = { { ShaderDataType::Float3, "a_Position" }, { ShaderDataType::Float2, "a_TexCoord" } };

    vbo->SetLayout(layout);

    p_VertexArray->AddVertexBuffer(vbo);

    std::shared_ptr<IndexBuffer> ibo = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

    p_VertexArray->SetIndexBuffer(ibo);
}

void CubeMesh::OnAwake() { p_Transform = m_Owner->GetComponent<Transform>(); }

void CubeMesh::OnRender()
{
    if (!p_Shader || !p_Texture || p_Transform == nullptr) return;

    Renderer::Submit(p_Shader, p_VertexArray, p_Texture, p_Transform->GetGlobalMatrix());
}

}