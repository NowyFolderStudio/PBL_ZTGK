#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/VertexArray.hpp>
#include "Renderer/RendererAPI.hpp"
#include "Renderer/Skybox.hpp"

namespace NFSEngine {
    struct RenderPacket {
        uint64_t sortKey = 0;
        ;
        std::shared_ptr<VertexArray> vao;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Texture> texture;
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

        static void BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                           const std::shared_ptr<Texture>& texture, const glm::mat4& transform);

        // TODO: Implement static Shutdown() method

        static RendererStats GetStats() { return s_Stats; }
        static float GetGPUTime();

        static void DrawSkybox(const std::shared_ptr<Skybox>& skybox, const std::shared_ptr<Shader>& shader);

    private:
        struct SceneData {
            glm::mat4 ViewMatrix;
            glm::mat4 ProjectionMatrix;
        };

        static SceneData* s_SceneData;
        static RendererStats s_Stats;

        static std::unique_ptr<GPUTimer> s_GPUTimer;
        static std::vector<RenderPacket> s_RendererQueue;
        static std::unique_ptr<RendererAPI> s_RendererAPI;

        static std::shared_ptr<VertexArray> s_SkyboxVAO;
    };
} // namespace NFSEngine
