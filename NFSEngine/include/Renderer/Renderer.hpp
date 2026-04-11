#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/VertexArray.hpp>
#include "Renderer/RendererAPI.hpp"

namespace NFSEngine
{
struct RenderPacket
{
    uint64_t sortKey = 0;
    ;
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    glm::mat4 transform = glm::mat4(1.0f);
};

class Renderer
{
public:
    static void Init();

    inline static RendererAPI& GetAPI() { return *s_RendererAPI; }

    static void BeginScene(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    static void EndScene();

    static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao,
                       const std::shared_ptr<Texture>& texture, const glm::mat4& transform);

private:
    struct SceneData
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };
    static SceneData* s_SceneData;

    static std::vector<RenderPacket> s_RendererQueue;
    static std::unique_ptr<RendererAPI> s_RendererAPI;
};
}
