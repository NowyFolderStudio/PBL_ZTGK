#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/VertexArray.hpp>
#include "Renderer/RendererAPI.hpp"
#include "Renderer/Skybox.hpp"
#include "Renderer/Frustum.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/Material.hpp"

namespace NFSEngine {
    struct RenderPacket {
        uint64_t sortKey = 0;

        std::shared_ptr<VertexArray> vao;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Material> material;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    struct RendererStats {
        uint32_t drawCalls = 0;
        uint32_t triangleCount = 0;
        uint32_t stateChanges = 0;
    };

    class GPUTimer;

    class Renderer {
    public:
        static void Init();

        inline static RendererAPI& GetAPI() { return *s_RendererAPI; }

        static void BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                           const std::shared_ptr<Material>& material, const glm::mat4& transform);

        // TODO: Implement static Shutdown() method

        static RendererStats GetStats() { return s_Stats; }
        static float GetGPUTime();

        static void DrawSkybox(const std::shared_ptr<Skybox>& skybox, const std::shared_ptr<Shader>& shader);
        static std::shared_ptr<VertexArray> GetSkyboxVAO() { return s_SkyboxVAO; }

        static glm::vec3 GetCameraPosition() { return s_SceneData->CameraPosition; }
        static const Frustum& GetFrustum() { return s_SceneData->frustum; }

        static void SetCullingRange(float range) { s_CullingRange = range; }
        static float GetCullingRange() { return s_CullingRange; }

        static void SetExposure(float exposure) { s_Exposure = exposure; }
        static float GetExposure() { return s_Exposure; }
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void SetFrustumCullingEnabled(bool enabled) { s_FrustumCullingEnabled = enabled; }
        static bool IsFrustumCullingEnabled() { return s_FrustumCullingEnabled; }
        static void SetFrustumCullingMode(int mode) { s_FrustumCullingMode = mode; }
        static int GetFrustumCullingMode() { return s_FrustumCullingMode; }

        static void SubmitDebugBox(const glm::mat4& transform, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f });

        static void SetDrawDebug(bool value);
        static void DrawDebug();

    private:
        struct SceneData {
            glm::mat4 ViewMatrix = glm::mat4(1.0f);
            glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
            glm::vec3 CameraPosition = glm::vec3(0.0f);
            Frustum frustum;
        };

        static SceneData* s_SceneData;
        static RendererStats s_Stats;

        static std::unique_ptr<GPUTimer> s_GPUTimer;
        static std::vector<RenderPacket> s_RendererQueue;
        static std::unique_ptr<RendererAPI> s_RendererAPI;

        static std::shared_ptr<VertexArray> s_SkyboxVAO;
        static float s_CullingRange;
        static bool s_FrustumCullingEnabled;
        static int s_FrustumCullingMode;

        static std::shared_ptr<Framebuffer> s_HDRFramebuffer;
        static std::shared_ptr<Shader> s_PostProcessShader;
        static float s_Exposure;

        struct DebugBox {
            glm::mat4 transform;
            glm::vec4 color;
        };

        static std::vector<DebugBox> s_DebugQueue;
        static std::shared_ptr<VertexArray> s_DebugCubeVAO;
        static std::shared_ptr<Shader> s_DebugShader;

        static bool s_DrawDebug;
    };
} // namespace NFSEngine
