#include "Renderer/Renderer.hpp"
#include <algorithm>

namespace NFSEngine
{

std::vector<RenderPacket> Renderer::s_RendererQueue;
std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

void Renderer::Init()
{
    s_RendererAPI = RendererAPI::Create();
    s_RendererAPI->Init();
}

void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    // TODO �adnie tu kamer� implementowa�
    s_SceneData->ViewMatrix = viewMatrix;
    s_SceneData->ProjectionMatrix = projectionMatrix;
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                      const std::shared_ptr<Texture>& texture, const glm::mat4& transform)
{
    RenderPacket packet;
    packet.vao = vao;
    packet.shader = shader;
    packet.texture = texture;
    packet.transform = transform;
    packet.sortKey = shader->GetRendererID();

    // TODO doda� optymalizacje renderowanie obiekt�w tworzenie id na podsawie tekstur, shadr�w

    s_RendererQueue.push_back(packet);
}

void Renderer::EndScene()
{
    std::sort(s_RendererQueue.begin(), s_RendererQueue.end(),
              [](const RenderPacket& a, const RenderPacket& b) { return a.sortKey < b.sortKey; });

    uint32_t lastShaderID = 0;

    for (const auto& packet : s_RendererQueue)
    {

        if (packet.shader->GetRendererID() != lastShaderID)
        {
            packet.shader->Bind();
            lastShaderID = packet.shader->GetRendererID();

            packet.shader->SetMat4("view", s_SceneData->ViewMatrix);
            packet.shader->SetMat4("projection", s_SceneData->ProjectionMatrix);
        }

        packet.shader->SetMat4("model", packet.transform);

        if (packet.texture)
        {
            packet.texture->Bind(0);
        }

        packet.vao->Bind();

        // s_RendererAPI->DrawIndexed(packet.VAO);
        s_RendererAPI->DrawArrays(packet.vao, 36);
    }

    s_RendererQueue.clear();
}
}