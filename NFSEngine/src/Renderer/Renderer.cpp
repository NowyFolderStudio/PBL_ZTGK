#include "Renderer/Renderer.hpp"
#include <algorithm>

namespace NFSEngine {

    std::vector<RenderPacket> Renderer::s_RendererQueue;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    void Renderer::Init() {
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        //TODO ³adnie tu kamerê implementowaæ
        s_SceneData->ViewMatrix = viewMatrix;
        s_SceneData->ProjectionMatrix = projectionMatrix;
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, const std::shared_ptr<Texture>& texture, const glm::mat4& transform) {
        RenderPacket packet;
        packet.VAO = vao;
        packet.Shader = shader;
        packet.Texture = texture;
        packet.Transform = transform;
        packet.SortKey = shader->GetRendererID();

        //TODO dodaæ optymalizacje renderowanie obiektów tworzenie id na podsawie tekstur, shadrów 

        s_RendererQueue.push_back(packet);
    }

    void Renderer::EndScene() {
        std::sort(s_RendererQueue.begin(), s_RendererQueue.end(), [](const RenderPacket& a, const RenderPacket& b) {
            return a.SortKey < b.SortKey;
            });

        uint32_t lastShaderID = 0;

        for (const auto& packet : s_RendererQueue) {

            if (packet.Shader->GetRendererID() != lastShaderID) {
                packet.Shader->Bind();
                lastShaderID = packet.Shader->GetRendererID();

                packet.Shader->SetMat4("view", s_SceneData->ViewMatrix);
                packet.Shader->SetMat4("projection", s_SceneData->ProjectionMatrix);
            }

            packet.Shader->SetMat4("model", packet.Transform);

            if (packet.Texture) {
                packet.Texture->Bind(0);
            }

            packet.VAO->Bind();

            //s_RendererAPI->DrawIndexed(packet.VAO);
            s_RendererAPI->DrawArrays(packet.VAO, 36);
        }

        s_RendererQueue.clear();
    }
}