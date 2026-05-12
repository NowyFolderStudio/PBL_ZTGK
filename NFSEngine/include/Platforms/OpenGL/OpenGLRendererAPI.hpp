#pragma once
#include "Renderer/RendererAPI.hpp"

namespace NFSEngine {
    class OpenGLRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
        virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void SetDepthTest(bool enabled) override;
        virtual void BindTexture(uint32_t textureID, uint32_t slot) override;
        virtual void DrawFullscreenTriangle() override;
        virtual void SetDepthFunction(DepthFunction func) override;
    };
} // namespace NFSEngine