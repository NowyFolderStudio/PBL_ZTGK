#pragma once
#include "Renderer/RendererAPI.hpp"

namespace NFSEngine {
    class OpenGLRendererAPI : public RendererAPI {
    public:
        void Init() override;
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;
        void ClearDepth() override;
        void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
        void DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount) override;
        void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetDepthTest(bool enabled) override;
        void BindTexture(uint32_t textureID, uint32_t slot) override;
        void DrawFullscreenTriangle() override;
        void SetDepthFunction(DepthFunction func) override;
        void SetWireframeMode(bool value) override;
        void SetBlendEnabled(bool enabled) override;
        void SetBlendFunction(BlendFunction func) override;
        void SetDepthWriteMask(bool enabled) override;
        void SetCullFace(bool value) override;
        void BindCubeTexture(uint32_t rendererID, uint32_t slot) override;
    };
} // namespace NFSEngine