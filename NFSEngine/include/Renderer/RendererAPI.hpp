#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Renderer/VertexArray.hpp"

namespace NFSEngine {
    enum class DepthFunction {
        Less,
        LEqual
    };

    class RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
        virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetDepthTest(bool enabled) = 0;
        virtual void BindTexture(uint32_t textureID, uint32_t slot) = 0;
        virtual void DrawFullscreenTriangle() = 0;
        virtual void SetDepthFunction(DepthFunction func) = 0;
        virtual void SetWireframeMode(bool value) = 0;

        static std::unique_ptr<RendererAPI> Create();
    };
} // namespace NFSEngine
