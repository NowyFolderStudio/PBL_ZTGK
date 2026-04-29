#include "Renderer/Renderer.hpp"
#include "Debug/Profiler.hpp"
#include "Debug/GPUTimer.hpp"
#include <algorithm>

namespace NFSEngine {

    std::vector<RenderPacket> Renderer::s_RendererQueue;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    RendererStats Renderer::s_Stats;
    std::unique_ptr<GPUTimer> Renderer::s_GPUTimer = nullptr;

    std::shared_ptr<VertexArray> Renderer::s_SkyboxVAO = nullptr;

    void Renderer::Init() {
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
        s_GPUTimer = std::make_unique<GPUTimer>();

        // Definicja wierzchołków sześcianu (zakres -1 do 1)
        float skyboxVertices[] = {
            -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
        };

        s_SkyboxVAO = std::shared_ptr<VertexArray>(VertexArray::Create());
        auto vbo = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices)));
        vbo->SetLayout({{ShaderDataType::Float3, "aPos"}});
        s_SkyboxVAO->AddVertexBuffer(vbo);
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        // TODO �adnie tu kamer� implementowa�
        s_SceneData->ViewMatrix = viewMatrix;
        s_SceneData->ProjectionMatrix = projectionMatrix;
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                          const std::shared_ptr<Texture>& texture, const glm::mat4& transform) {
        RenderPacket packet;
        packet.vao = vao;
        packet.shader = shader;
        packet.texture = texture;
        packet.transform = transform;
        packet.sortKey = shader->GetRendererID();

        // TODO doda� optymalizacje renderowanie obiekt�w tworzenie id na podsawie tekstur, shadr�w

        s_RendererQueue.push_back(packet);
    }

    void Renderer::EndScene() {
        NFS_PROFILE_FUNCTION();

        s_Stats.drawCalls = 0;
        s_Stats.triangleCount = 0;
        s_Stats.stateChanges = 0;

        std::sort(s_RendererQueue.begin(), s_RendererQueue.end(),
                  [](const RenderPacket& a, const RenderPacket& b) { return a.sortKey < b.sortKey; });

        uint32_t lastShaderID = 0;
        uint32_t lastTextureID = 0;

        s_GPUTimer->Begin();

        for (const auto& packet : s_RendererQueue) {

            if (packet.shader->GetRendererID() != lastShaderID) {
                packet.shader->Bind();
                lastShaderID = packet.shader->GetRendererID();

                s_Stats.stateChanges++;

                packet.shader->SetMat4("view", s_SceneData->ViewMatrix);
                packet.shader->SetMat4("projection", s_SceneData->ProjectionMatrix);
            }

            packet.shader->SetMat4("model", packet.transform);

            if (packet.texture) {
                if (packet.texture->GetRendererID() != lastTextureID) {
                    packet.texture->Bind(0);
                    lastTextureID = packet.texture->GetRendererID();

                    s_Stats.stateChanges++;
                }
            }

            packet.vao->Bind();
            s_Stats.stateChanges++;

            s_RendererAPI->DrawIndexed(packet.vao);
            s_Stats.drawCalls++;

            if (packet.vao->GetIndexBuffer()) {
                s_Stats.triangleCount += packet.vao->GetIndexBuffer()->GetCount() / 3;
            }
        }

        s_GPUTimer->End();

        s_RendererQueue.clear();
    }

    float Renderer::GetGPUTime() { return s_GPUTimer ? s_GPUTimer->GetTimeMS() : 0.0f; }

    void Renderer::DrawSkybox(const std::shared_ptr<Skybox>& skybox, const std::shared_ptr<Shader>& shader) {
        if (!skybox || !shader) return;

        glDepthFunc(GL_LEQUAL);

        shader->Bind();
        shader->SetMat4("view", glm::mat4(glm::mat3(s_SceneData->ViewMatrix)));
        shader->SetMat4("projection", s_SceneData->ProjectionMatrix);

        skybox->Bind(0);
        shader->SetInt("skybox", 0);

        s_SkyboxVAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
    }

} // namespace NFSEngine