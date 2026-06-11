#include <algorithm>
#include <string>

#include "Renderer/Renderer.hpp"
#include "Debug/Profiler.hpp"
#include "Debug/GPUTimer.hpp"
#include "Core/Application.hpp"

#include "Renderer/EnvironmentMap.hpp"
#include "Components/DirectionalLight.hpp"
#include "Components/PointLight.hpp"
#include "Components/SpotLight.hpp"
#include "Renderer/Framebuffer.hpp"

namespace NFSEngine {

    std::vector<RenderPacket> Renderer::s_RendererQueue;
    std::vector<InstancedRenderPacket> Renderer::s_InstancedQueue;
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

    std::vector<std::shared_ptr<Framebuffer>> Renderer::s_BloomFBOs;
    std::shared_ptr<Shader> Renderer::s_DownsampleShader = nullptr;
    std::shared_ptr<Shader> Renderer::s_UpsampleShader = nullptr;

    std::shared_ptr<Framebuffer> Renderer::s_ShadowMapFBO = nullptr;
    std::shared_ptr<Shader> Renderer::s_ShadowShader = nullptr;
    std::shared_ptr<Shader> Renderer::s_AnimatedShadowShader = nullptr;
    glm::mat4 Renderer::s_LightSpaceMatrix = glm::mat4(1.0f);

    std::shared_ptr<Framebuffer> Renderer::s_PointShadowMapFBO = nullptr;
    std::shared_ptr<Shader> Renderer::s_PointShadowShader = nullptr;
    std::shared_ptr<Shader> Renderer::s_PointShadowShaderAnim = nullptr;

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

        // clang-format off
        fbSpec.attachments = {
            FramebufferTextureFormat::RGBA16F,          // Main colors
            FramebufferTextureFormat::RGBA16F,          // Bloom
            FramebufferTextureFormat::RGBA8,            // Outline colors
            FramebufferTextureFormat::RGBA16F,          // Normal vectors
            FramebufferTextureFormat::RGBA16F,          // Outline parameters
            FramebufferTextureFormat::DEPTH24STENCIL8   // Depth
        };
        // clang-format on

        s_HDRFramebuffer = Framebuffer::Create(fbSpec);

        FramebufferSpecification pingPongSpec;
        pingPongSpec.width = Application::Get().GetConfig().WindowWidth;
        pingPongSpec.height = Application::Get().GetConfig().WindowHeight;
        pingPongSpec.attachments = { FramebufferTextureFormat::RGBA16F };

        s_DownsampleShader = Shader::Create("Downsample", "assets/shaders/postprocess.vert", "assets/shaders/downsample.frag");
        s_UpsampleShader = Shader::Create("Upsample", "assets/shaders/postprocess.vert", "assets/shaders/upsample.frag");

        SetupBloomChain(Application::Get().GetConfig().WindowWidth, Application::Get().GetConfig().WindowHeight);

        s_PostProcessShader = Shader::Create("PostProcess", "assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag");

        FramebufferSpecification shadowSpec;
        shadowSpec.width = 4096;
        shadowSpec.height = 4096;
        shadowSpec.attachments = { FramebufferTextureFormat::DEPTH_COMPONENT };

        s_ShadowMapFBO = Framebuffer::Create(shadowSpec);
        s_ShadowShader = Shader::Create("ShadowShader", "assets/shaders/shadowMap.vert", "assets/shaders/shadowMap.frag");
        s_AnimatedShadowShader = Shader::Create("AnimShadowShader", "assets/shaders/shadowMapAnimated.vert", "assets/shaders/shadowMap.frag");

        FramebufferSpecification pointShadowSpec;
        pointShadowSpec.width = 2048;
        pointShadowSpec.height = 2048;
        pointShadowSpec.attachments = { FramebufferTextureFormat::DEPTH_CUBEMAP };

        s_PointShadowMapFBO = Framebuffer::Create(pointShadowSpec);

        s_PointShadowShader = Shader::Create("PointShadowShader",
            "assets/shaders/pointShadow.vert",
            "assets/shaders/pointShadow.frag",
            "assets/shaders/pointShadow.geom");

        s_PointShadowShaderAnim = Shader::Create("PointShadowShaderAnim",
            "assets/shaders/pointShadowAnimated.vert",
            "assets/shaders/pointShadow.frag",
            "assets/shaders/pointShadow.geom");

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

        SetupBloomChain(width, height);
    }

    void Renderer::BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition,
                              DirectionalLight* dirLight, const std::vector<PointLight*>& pointLights,
                              const std::vector<SpotLight*>& spotLights, EnvironmentMap* envMap) {
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
                          const std::shared_ptr<Material>& material, const glm::mat4& transform,
                          const std::vector<glm::mat4>& boneTransforms) {
        RenderPacket packet;
        packet.vao = vao;
        packet.shader = shader;
        packet.material = material;
        packet.transform = transform;
        packet.sortKey = shader->GetRendererID();
        packet.boneTransforms = boneTransforms;

        // TODO doda閿燂拷 optymalizacje renderowanie obiekt閿熺弹 tworzenie id na podsawie tekstur, shadr閿熺弹

        s_RendererQueue.push_back(packet);
    }

    void Renderer::SubmitInstanced(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                                   const std::shared_ptr<Material>& material, uint32_t instanceCount) {
        if (instanceCount == 0) return;

        InstancedRenderPacket packet;
        packet.vao = vao;
        packet.shader = shader;
        packet.material = material;
        packet.instanceCount = instanceCount;
        packet.sortKey = shader->GetRendererID();

        s_InstancedQueue.push_back(packet);
    }

    void Renderer::EndScene() {
        NFS_PROFILE_FUNCTION();

        s_Stats.drawCalls = 0;
        s_Stats.triangleCount = 0;
        s_Stats.stateChanges = 0;

        s_GPUTimer->Begin();

        std::sort(s_RendererQueue.begin(), s_RendererQueue.end(),
                  [](const RenderPacket& a, const RenderPacket& b) { return a.sortKey < b.sortKey; });

        std::sort(s_InstancedQueue.begin(), s_InstancedQueue.end(),
            [](const InstancedRenderPacket& a, const InstancedRenderPacket& b) { return a.sortKey < b.sortKey; });

        if (s_SceneData->DirLight) {
            float orthoSize = 120.0f;
            float shadowMapRes = (float)s_ShadowMapFBO->GetSpecification().width;

            float texelSize = (orthoSize * 2.0f) / shadowMapRes;

            glm::vec3 targetPos = s_SceneData->CameraPosition;
            glm::vec3 lightDir = glm::normalize(s_SceneData->DirLight->Direction);
            glm::vec3 lightPos = targetPos - (lightDir * 50.0f);

            glm::mat4 lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));

            glm::vec4 originInLightSpace = lightView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            glm::vec3 snappedOrigin;
            snappedOrigin.x = std::floor(originInLightSpace.x / texelSize) * texelSize;
            snappedOrigin.y = std::floor(originInLightSpace.y / texelSize) * texelSize;
            snappedOrigin.z = originInLightSpace.z;

            glm::vec3 offset = glm::vec3(originInLightSpace) - snappedOrigin;
            float left = -orthoSize + offset.x;
            float right = orthoSize + offset.x;
            float bottom = -orthoSize + offset.y;
            float top = orthoSize + offset.y;
            float zNear = 1.0f;
            float zFar = 250.0f;

            glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, zNear, zFar);

            s_LightSpaceMatrix = lightProjection * lightView;

            s_ShadowMapFBO->Bind();

            s_RendererAPI->SetViewport(0, 0, s_ShadowMapFBO->GetSpecification().width, s_ShadowMapFBO->GetSpecification().height);
            s_RendererAPI->ClearDepth();
            s_RendererAPI->SetDepthTest(true);

            //s_RendererAPI->SetCullFace(1);// Fix for peter panning // Not working properly, requires further investigation

            
            // Check optimization of this function //TODOgugapl
            for (const auto& packet : s_RendererQueue) {
                if (!packet.boneTransforms.empty()) {
                    s_AnimatedShadowShader->Bind();
                    s_AnimatedShadowShader->SetMat4("lightSpaceMatrix", s_LightSpaceMatrix);
                    s_AnimatedShadowShader->SetMat4("model", packet.transform);

                    for (int i = 0; i < packet.boneTransforms.size(); i++) {
                        s_AnimatedShadowShader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", packet.boneTransforms[i]);
                    }
                }
                else {
                    s_ShadowShader->Bind();
                    s_ShadowShader->SetMat4("lightSpaceMatrix", s_LightSpaceMatrix);
                    s_ShadowShader->SetMat4("model", packet.transform);
                }

                packet.vao->Bind();
                s_RendererAPI->DrawIndexed(packet.vao);
            }

            for (const auto& packet : s_InstancedQueue) {
                packet.vao->Bind();
                s_RendererAPI->DrawIndexedInstanced(packet.vao, packet.instanceCount);
            }

            //s_RendererAPI->SetCullFace(0);

            s_ShadowMapFBO->Unbind();
        }

        if (s_SceneData->PointLights && !s_SceneData->PointLights->empty()) {
            auto* light = (*s_SceneData->PointLights)[0];
            glm::vec3 lightPos = light->GetOwner()->GetTransform()->GetPosition();

            // Change this parameter to work with point light //TODOgugapl
            float farPlane = 25.0f;

            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);

            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

            s_PointShadowMapFBO->Bind();
            s_RendererAPI->SetViewport(0, 0, s_PointShadowMapFBO->GetSpecification().width, s_PointShadowMapFBO->GetSpecification().height);
            s_RendererAPI->ClearDepth();
            s_RendererAPI->SetDepthTest(true);

            s_PointShadowShader->Bind();
            s_PointShadowShader->SetMat4Array("shadowMatrices", shadowTransforms);
            s_PointShadowShader->SetVec3("lightPos", lightPos);
            s_PointShadowShader->SetFloat("far_plane", farPlane);

            // Check optimization of this function //TODOgugapl

            for (const auto& packet : s_RendererQueue) {
                if (!packet.boneTransforms.empty()) continue;

                s_PointShadowShader->SetMat4("model", packet.transform);
                packet.vao->Bind();
                s_RendererAPI->DrawIndexed(packet.vao);
            }

            s_PointShadowShaderAnim->Bind();
            s_PointShadowShaderAnim->SetMat4Array("shadowMatrices", shadowTransforms);
            s_PointShadowShaderAnim->SetVec3("lightPos", lightPos);
            s_PointShadowShaderAnim->SetFloat("far_plane", farPlane);

            for (const auto& packet : s_RendererQueue) {
                if (packet.boneTransforms.empty()) continue;

                s_PointShadowShaderAnim->SetMat4("model", packet.transform);
                for (int i = 0; i < packet.boneTransforms.size(); i++) {
                    s_PointShadowShaderAnim->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", packet.boneTransforms[i]);
                }
                packet.vao->Bind();
                s_RendererAPI->DrawIndexed(packet.vao);
            }

            s_PointShadowMapFBO->Unbind();
        }

        if (s_HDRFramebuffer->GetSpecification().width > 0 && s_HDRFramebuffer->GetSpecification().height > 0) {
            s_HDRFramebuffer->Bind();
            s_RendererAPI->SetViewport(0, 0, s_HDRFramebuffer->GetSpecification().width, s_HDRFramebuffer->GetSpecification().height);
        }

        uint32_t lastShaderID = 0;
        uint32_t lastTextureID = 0;

        {
            NFS_PROFILE_SCOPE("Render Queue");
            for (const auto& packet : s_RendererQueue) {
                if (packet.shader->GetRendererID() != lastShaderID) {
                    packet.shader->Bind();
                    lastShaderID = packet.shader->GetRendererID();

                    s_Stats.stateChanges++;

                    packet.shader->SetMat4("view", s_SceneData->ViewMatrix);
                    packet.shader->SetMat4("projection", s_SceneData->ProjectionMatrix);
                    packet.shader->SetVec3("viewPos", s_SceneData->CameraPosition);

                    packet.shader->SetMat4("lightSpaceMatrix", s_LightSpaceMatrix);
                    s_RendererAPI->BindTexture(s_ShadowMapFBO->GetDepthAttachmentRendererID(), 7);
                    packet.shader->SetInt("shadowMap", 7);

                    s_RendererAPI->BindCubeTexture(s_PointShadowMapFBO->GetDepthAttachmentRendererID(), 8);
                    packet.shader->SetInt("pointShadowMap", 8);
                    packet.shader->SetFloat("pointShadowFarPlane", 25.0f);

                    if (s_SceneData->EnvMap) {
                        s_SceneData->EnvMap->BindEnvironmentMaps(30, 29, 28);
                        packet.shader->SetInt("irradianceMap", 30);
                        packet.shader->SetInt("prefilterMap", 29);
                        packet.shader->SetInt("brdfLUT", 28);
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

                if (!packet.boneTransforms.empty()) {
                    for (int i = 0; i < packet.boneTransforms.size(); i++) {
                        packet.shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", packet.boneTransforms[i]);
                    }
                }

                if (packet.material) {
                    packet.material->Bind(packet.shader);

                    for (const auto& [name, value] : packet.material->Properties) {
                        std::visit(
                            [&](auto&& arg) {
                                using T = std::decay_t<decltype(arg)>;
                                if constexpr (std::is_same_v<T, float>)
                                    packet.shader->SetFloat(name, arg);
                                else if constexpr (std::is_same_v<T, int>)
                                    packet.shader->SetInt(name, arg);
                                else if constexpr (std::is_same_v<T, glm::vec3>)
                                    packet.shader->SetVec3(name, arg);
                                else if constexpr (std::is_same_v<T, glm::vec4>)
                                    packet.shader->SetVec4(name, arg);
                            },
                            value);
                    }

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
        }

        {
            NFS_PROFILE_SCOPE("Instanced Render Queue");
            s_RendererAPI->SetBlendEnabled(true);
            s_RendererAPI->SetBlendFunction(BlendFunction::Alpha);
            for (const auto& packet : s_InstancedQueue) {

                if (packet.shader->GetRendererID() != lastShaderID) {
                    packet.shader->Bind();
                    lastShaderID = packet.shader->GetRendererID();
                    s_Stats.stateChanges++;

                    packet.shader->SetMat4("view", s_SceneData->ViewMatrix);
                    packet.shader->SetMat4("projection", s_SceneData->ProjectionMatrix);
                    packet.shader->SetVec3("viewPos", s_SceneData->CameraPosition);

                    packet.shader->SetMat4("lightSpaceMatrix", s_LightSpaceMatrix);
                    s_RendererAPI->BindTexture(s_ShadowMapFBO->GetDepthAttachmentRendererID(), 7);
                    packet.shader->SetInt("shadowMap", 7);

                    s_RendererAPI->BindCubeTexture(s_PointShadowMapFBO->GetDepthAttachmentRendererID(), 8);
                    packet.shader->SetInt("pointShadowMap", 8);
                    packet.shader->SetFloat("pointShadowFarPlane", 25.0f);

                    if (s_SceneData->EnvMap) {
                        s_SceneData->EnvMap->BindEnvironmentMaps(30, 29, 28);
                        packet.shader->SetInt("irradianceMap", 30);
                        packet.shader->SetInt("prefilterMap", 29);
                        packet.shader->SetInt("brdfLUT", 28);
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

                if (packet.material) {
                    packet.material->Bind(packet.shader);

                    for (const auto& [name, value] : packet.material->Properties) {
                        std::visit(
                            [&](auto&& arg) {
                                using T = std::decay_t<decltype(arg)>;
                                if constexpr (std::is_same_v<T, float>)
                                    packet.shader->SetFloat(name, arg);
                                else if constexpr (std::is_same_v<T, int>)
                                    packet.shader->SetInt(name, arg);
                                else if constexpr (std::is_same_v<T, glm::vec3>)
                                    packet.shader->SetVec3(name, arg);
                                else if constexpr (std::is_same_v<T, glm::vec4>)
                                    packet.shader->SetVec4(name, arg);
                            },
                            value);
                    }

                    s_Stats.stateChanges++;
                }

                packet.vao->Bind();
                s_Stats.stateChanges++;

                s_RendererAPI->DrawIndexedInstanced(packet.vao, packet.instanceCount);
                s_Stats.drawCalls++;

                if (packet.vao->GetIndexBuffer()) {
                    s_Stats.triangleCount += (packet.vao->GetIndexBuffer()->GetCount() / 3) * packet.instanceCount;
                }
            }
            s_RendererAPI->SetBlendEnabled(false);
        }

        DrawDebug();

        s_GPUTimer->End();
        s_RendererQueue.clear();
        s_InstancedQueue.clear();

        s_HDRFramebuffer->Unbind();

        s_RendererAPI->SetDepthTest(false);

        s_DownsampleShader->Bind();
        uint32_t currentTexture = s_HDRFramebuffer->GetColorAttachmentRendererID(1);

        for (size_t i = 0; i < s_BloomFBOs.size(); i++) {
            s_BloomFBOs[i]->Bind();

            glm::vec2 mipSize = { s_BloomFBOs[i]->GetSpecification().width, s_BloomFBOs[i]->GetSpecification().height };

            glm::vec2 srcRes;
            if (i == 0)
                srcRes = { s_HDRFramebuffer->GetSpecification().width, s_HDRFramebuffer->GetSpecification().height };
            else
                srcRes = { s_BloomFBOs[i - 1]->GetSpecification().width, s_BloomFBOs[i - 1]->GetSpecification().height };

            s_DownsampleShader->SetVec2("srcResolution", srcRes);

            s_RendererAPI->BindTexture(currentTexture, 0);
            s_DownsampleShader->SetInt("srcTexture", 0);

            s_RendererAPI->DrawFullscreenTriangle();

            currentTexture = s_BloomFBOs[i]->GetColorAttachmentRendererID(0);
        }

        s_UpsampleShader->Bind();
        s_UpsampleShader->SetFloat("filterRadius", 0.005f);

        s_RendererAPI->SetBlendEnabled(true);
        s_RendererAPI->SetBlendFunction(BlendFunction::Additive);

        for (int i = s_BloomFBOs.size() - 2; i >= 0; i--) {
            s_BloomFBOs[i]->Bind();

            uint32_t textureToUpsample = s_BloomFBOs[i + 1]->GetColorAttachmentRendererID(0);

            s_RendererAPI->BindTexture(textureToUpsample, 0);
            s_UpsampleShader->SetInt("srcTexture", 0);

            s_RendererAPI->DrawFullscreenTriangle();
        }

        s_RendererAPI->SetBlendFunction(BlendFunction::Alpha);
        s_BloomFBOs[0]->Unbind();

        uint32_t screenWidth = s_HDRFramebuffer->GetSpecification().width;
        uint32_t screenHeight = s_HDRFramebuffer->GetSpecification().height;
        s_RendererAPI->SetViewport(0, 0, screenWidth, screenHeight);

        s_PostProcessShader->Bind();
        s_PostProcessShader->SetFloat("exposure", s_Exposure);

        s_RendererAPI->BindTexture(s_HDRFramebuffer->GetColorAttachmentRendererID(0), 0);
        s_PostProcessShader->SetInt("screenTexture", 0);

        s_RendererAPI->BindTexture(s_BloomFBOs[0]->GetColorAttachmentRendererID(0), 1);
        s_PostProcessShader->SetInt("bloomBlurTexture", 1);

        s_RendererAPI->BindTexture(s_HDRFramebuffer->GetColorAttachmentRendererID(2), 2);
        s_PostProcessShader->SetInt("outlineColorTexture", 2);

        s_RendererAPI->BindTexture(s_HDRFramebuffer->GetDepthAttachmentRendererID(), 3);
        s_PostProcessShader->SetInt("depthTexture", 3);

        s_RendererAPI->BindTexture(s_HDRFramebuffer->GetColorAttachmentRendererID(3), 4);
        s_PostProcessShader->SetInt("normalTexture", 4);

        s_RendererAPI->BindTexture(s_HDRFramebuffer->GetColorAttachmentRendererID(4), 5);
        s_PostProcessShader->SetInt("outlineParamsTexture", 5);

        s_RendererAPI->DrawFullscreenTriangle();

        s_RendererAPI->SetDepthTest(true);
    }

    void Renderer::SetupBloomChain(uint32_t width, uint32_t height) {
        s_BloomFBOs.clear();

        glm::vec2 mipSize(width, height);
        for (int i = 0; i < 6; i++) {
            mipSize *= 0.5f;

            if (mipSize.x < 1.0f) mipSize.x = 1.0f;
            if (mipSize.y < 1.0f) mipSize.y = 1.0f;

            FramebufferSpecification spec;
            spec.width = (uint32_t)mipSize.x;
            spec.height = (uint32_t)mipSize.y;
            spec.attachments = { FramebufferTextureFormat::RGBA16F };

            s_BloomFBOs.push_back(Framebuffer::Create(spec));
        }
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