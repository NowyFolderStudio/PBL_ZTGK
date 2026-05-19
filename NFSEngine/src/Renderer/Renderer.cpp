#include <algorithm>

#include "Renderer/Renderer.hpp"
#include "Debug/Profiler.hpp"
#include "Debug/GPUTimer.hpp"
#include "Core/Application.hpp"

#include "Renderer/EnvironmentMap.hpp"
#include "Components/DirectionalLight.hpp"
#include "Components/PointLight.hpp"
#include "Components/SpotLight.hpp"

namespace NFSEngine {

    std::vector<RenderPacket> Renderer::s_RendererQueue;
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

    RendererStats Renderer::s_Stats;
    std::unique_ptr<GPUTimer> Renderer::s_GPUTimer = nullptr;

    std::shared_ptr<VertexArray> Renderer::s_SkyboxVAO = nullptr;

    float Renderer::s_CullingRange = 0.0f;
    bool Renderer::s_FrustumCullingEnabled = true;
    int Renderer::s_FrustumCullingMode = 0;

    std::shared_ptr<Framebuffer> Renderer::s_HDRFramebuffer = nullptr;
    std::shared_ptr<Shader> Renderer::s_PostProcessShader = nullptr;
    float Renderer::s_Exposure = 1.0f;

    std::vector<Renderer::DebugBox> Renderer::s_DebugQueue;
    std::shared_ptr<VertexArray> Renderer::s_DebugCubeVAO;
    std::shared_ptr<Shader> Renderer::s_DebugShader;

    bool Renderer::s_DrawDebug;

    void Renderer::Init() {
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
        s_GPUTimer = std::make_unique<GPUTimer>();
        s_DrawDebug = false;

        FramebufferSpecification fbSpec;
        fbSpec.width = Application::Get().GetConfig().WindowWidth;
        fbSpec.height = Application::Get().GetConfig().WindowHeight;
        fbSpec.attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH24STENCIL8 };

        s_HDRFramebuffer = Framebuffer::Create(fbSpec);

        s_PostProcessShader = Shader::Create("PostProcess", "assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag");

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

        s_DebugShader = Shader::Create("DebugShader", "assets/shaders/debug.vert", "assets/shaders/debug.frag");

        float cubeVertices[] = { -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
                                 -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f };

        uint32_t cubeIndices[]
            = { 0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 5, 4, 7, 7, 6, 5, 4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4 };

        s_DebugCubeVAO = std::shared_ptr<VertexArray>(VertexArray::Create());
        auto debugVBO = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));
        debugVBO->SetLayout({ { ShaderDataType::Float3, "aPos" } });
        auto debugIBO = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));

        s_DebugCubeVAO->AddVertexBuffer(debugVBO);
        s_DebugCubeVAO->SetIndexBuffer(debugIBO);
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        if (s_HDRFramebuffer) {
            s_HDRFramebuffer->Resize(width, height);
        }
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        const glm::vec3& cameraPosition, 
        DirectionalLight* dirLight,
        const std::vector<PointLight*>& pointLights,
        const std::vector<SpotLight*>& spotLights,
        EnvironmentMap* envMap) {
        s_SceneData->ViewMatrix = viewMatrix;
        s_SceneData->ProjectionMatrix = projectionMatrix;
        s_SceneData->CameraPosition = cameraPosition;
        s_SceneData->frustum.ExtractFromMatrix(projectionMatrix * viewMatrix);
        s_SceneData->DirLight = dirLight;
        s_SceneData->PointLights = &pointLights;
        s_SceneData->SpotLights = &spotLights;
        s_SceneData->EnvMap = envMap;

        if (s_HDRFramebuffer->GetSpecification().width > 0 && s_HDRFramebuffer->GetSpecification().height > 0) {
            s_HDRFramebuffer->Bind();
        }

        s_RendererAPI->SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
        s_RendererAPI->Clear();
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                          const std::shared_ptr<Material>& material, const glm::mat4& transform) {
        RenderPacket packet;
        packet.vao = vao;
        packet.shader = shader;
        packet.material = material;
        packet.transform = transform;
        packet.sortKey = shader->GetRendererID();

        // TODO doda閿燂拷 optymalizacje renderowanie obiekt閿熺弹 tworzenie id na podsawie tekstur, shadr閿熺弹

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

                packet.shader->SetVec3("viewPos", s_SceneData->CameraPosition);

                if (s_SceneData->EnvMap) {
                    s_SceneData->EnvMap->BindEnvironmentMaps(5, 6, 7);
                    packet.shader->SetInt("irradianceMap", 5);
                    packet.shader->SetInt("prefilterMap", 6);
                    packet.shader->SetInt("brdfLUT", 7);
                }

                if (s_SceneData->DirLight) {
                    packet.shader->SetVec3("dirLight.direction", s_SceneData->DirLight->Direction);
                    packet.shader->SetVec3("dirLight.color", s_SceneData->DirLight->Color);
                    packet.shader->SetFloat("dirLight.intensity", s_SceneData->DirLight->Intensity);
                }

                if (s_SceneData->PointLights) {
                    int lightIndex = 0;
                    for (auto* light : *s_SceneData->PointLights) {
                        if (lightIndex >= 16) break;
                        std::string base = "pointLights[" + std::to_string(lightIndex) + "].";
                        packet.shader->SetVec3(base + "position", light->GetOwner()->GetTransform()->GetPosition());
                        packet.shader->SetVec3(base + "color", light->Color);
                        packet.shader->SetFloat(base + "intensity", light->Intensity);
                        packet.shader->SetFloat(base + "constant", light->Constant);
                        packet.shader->SetFloat(base + "linear", light->Linear);
                        packet.shader->SetFloat(base + "quadratic", light->Quadratic);
                        lightIndex++;
                    }
                    packet.shader->SetInt("activePointLights", lightIndex);
                }

                if (s_SceneData->SpotLights) {
                    int spotIndex = 0;
                    for (auto* light : *s_SceneData->SpotLights) {
                        if (spotIndex >= 4) break;
                        std::string base = "spotLights[" + std::to_string(spotIndex) + "].";
                        packet.shader->SetVec3(base + "position", light->GetOwner()->GetTransform()->GetPosition());
                        packet.shader->SetVec3(base + "direction", light->Direction);
                        packet.shader->SetVec3(base + "color", light->Color);
                        packet.shader->SetFloat(base + "intensity", light->Intensity);
                        packet.shader->SetFloat(base + "cutOff", light->CutOff);
                        packet.shader->SetFloat(base + "outerCutOff", light->OuterCutOff);
                        packet.shader->SetFloat(base + "constant", light->Constant);
                        packet.shader->SetFloat(base + "linear", light->Linear);
                        packet.shader->SetFloat(base + "quadratic", light->Quadratic);
                        spotIndex++;
                    }
                    packet.shader->SetInt("activeSpotLights", spotIndex);
                }
            }

            packet.shader->SetMat4("model", packet.transform);

            if (packet.material) {
                packet.material->Bind(packet.shader);

                s_Stats.stateChanges++;
            }

            packet.vao->Bind();
            s_Stats.stateChanges++;

            s_RendererAPI->DrawIndexed(packet.vao);
            s_Stats.drawCalls++;

            if (packet.vao->GetIndexBuffer()) {
                s_Stats.triangleCount += packet.vao->GetIndexBuffer()->GetCount() / 3;
            }
        }

        DrawDebug();

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

    void Renderer::SubmitDebugBox(const glm::mat4& transform, const glm::vec4& color) {
        s_DebugQueue.push_back({ transform, color });
    }

    void Renderer::SetDrawDebug(bool value) { s_DrawDebug = value; };

    void Renderer::DrawDebug() {
        if (!s_DebugQueue.empty() && s_DrawDebug) {
            s_DebugShader->Bind();
            s_DebugShader->SetMat4("view", s_SceneData->ViewMatrix);
            s_DebugShader->SetMat4("projection", s_SceneData->ProjectionMatrix);

            s_RendererAPI->SetWireframeMode(true);
            s_RendererAPI->SetDepthTest(false);

            s_DebugCubeVAO->Bind();

            for (const auto& box : s_DebugQueue) {
                s_DebugShader->SetMat4("model", box.transform);
                s_DebugShader->SetVec4("u_Color", box.color);

                s_RendererAPI->DrawIndexed(s_DebugCubeVAO);
                s_Stats.drawCalls++;
            }

            s_RendererAPI->SetWireframeMode(false);
            s_RendererAPI->SetDepthTest(true);
            s_DebugQueue.clear();
        }
    }

} // namespace NFSEngine