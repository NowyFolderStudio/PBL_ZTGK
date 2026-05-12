#include "Renderer/Renderer.hpp"
#include "Debug/Profiler.hpp"
#include "Debug/GPUTimer.hpp"
#include "Core/Application.hpp"
#include <algorithm>

namespace NFSEngine {

    std::vector<RenderPacket> Renderer::s_RendererQueue;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    RendererStats Renderer::s_Stats;
    std::unique_ptr<GPUTimer> Renderer::s_GPUTimer = nullptr;

    std::shared_ptr<VertexArray> Renderer::s_SkyboxVAO = nullptr;

    float Renderer::s_CullingRange = 0.0f;

    std::shared_ptr<Framebuffer> Renderer::s_HDRFramebuffer = nullptr;
    std::shared_ptr<Shader> Renderer::s_PostProcessShader = nullptr;
    float Renderer::s_Exposure = 1.0f;

    void Renderer::Init() {
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
        s_GPUTimer = std::make_unique<GPUTimer>();

<<<<<<< HEAD
        // Definicja wierzcho艂k贸w sze艣cianu (zakres -1 do 1)
=======
        FramebufferSpecification fbSpec;
        fbSpec.width = Application::Get().GetConfig().WindowWidth;
        fbSpec.height = Application::Get().GetConfig().WindowHeight;
        fbSpec.attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH24STENCIL8 };

        s_HDRFramebuffer = Framebuffer::Create(fbSpec);

        s_PostProcessShader = Shader::Create("PostProcess", "assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag");

>>>>>>> refs/remotes/origin/main
        float skyboxVertices[] = { -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                                   1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                                   -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                                   -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                                   1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                                   1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                                   -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                                   1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                                   -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                                   1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                                   -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                                   1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f };

        s_SkyboxVAO = std::shared_ptr<VertexArray>(VertexArray::Create());
        auto vbo = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices)));
        vbo->SetLayout({ { ShaderDataType::Float3, "aPos" } });
        s_SkyboxVAO->AddVertexBuffer(vbo);
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        if (s_HDRFramebuffer) {
            s_HDRFramebuffer->Resize(width, height);
        }
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition) {
<<<<<<< HEAD
        // TODO 锟絘dnie tu kamer锟� implementowa锟�
=======
>>>>>>> refs/remotes/origin/main
        s_SceneData->ViewMatrix = viewMatrix;
        s_SceneData->ProjectionMatrix = projectionMatrix;
        s_SceneData->CameraPosition = cameraPosition;
        s_SceneData->Frustum.ExtractFromMatrix(projectionMatrix * viewMatrix);

        if (s_HDRFramebuffer->GetSpecification().width > 0 && s_HDRFramebuffer->GetSpecification().height > 0) {
            s_HDRFramebuffer->Bind();
        }

        s_RendererAPI->SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
        s_RendererAPI->Clear();
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                          const std::shared_ptr<Texture>& texture, const glm::mat4& transform) {
        RenderPacket packet;
        packet.vao = vao;
        packet.shader = shader;
        packet.texture = texture;
        packet.transform = transform;
        packet.sortKey = shader->GetRendererID();

        // TODO doda锟� optymalizacje renderowanie obiekt锟絯 tworzenie id na podsawie tekstur, shadr锟絯

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

        s_HDRFramebuffer->Unbind();

        s_RendererAPI->SetDepthTest(false);

        s_PostProcessShader->Bind();
        s_PostProcessShader->SetFloat("exposure", s_Exposure);
        s_PostProcessShader->SetInt("screenTexture", 0);

        uint32_t colorTextureID = s_HDRFramebuffer->GetColorAttachmentRendererID(0);
        s_RendererAPI->BindTexture(colorTextureID, 0);

        s_RendererAPI->DrawFullscreenTriangle();

        s_RendererAPI->SetDepthTest(true);
    }

    float Renderer::GetGPUTime() { return s_GPUTimer ? s_GPUTimer->GetTimeMS() : 0.0f; }

    void Renderer::DrawSkybox(const std::shared_ptr<Skybox>& skybox, const std::shared_ptr<Shader>& shader) {
        if (!skybox || !shader) return;

        s_RendererAPI->SetDepthFunction(DepthFunction::LEqual);

        shader->Bind();
        shader->SetMat4("view", glm::mat4(glm::mat3(s_SceneData->ViewMatrix)));
        shader->SetMat4("projection", s_SceneData->ProjectionMatrix);

        skybox->Bind(0);
        shader->SetInt("skybox", 0);

        s_SkyboxVAO->Bind();
        s_RendererAPI->DrawArrays(s_SkyboxVAO, 36);

        s_RendererAPI->SetDepthFunction(DepthFunction::Less);
    }

} // namespace NFSEngine