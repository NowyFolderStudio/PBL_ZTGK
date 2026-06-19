#include "Platforms/OpenGL/OpenGLRendererAPI.hpp"
#include <glad/glad.h>

namespace NFSEngine {
    void OpenGLRendererAPI::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

    void OpenGLRendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

    void OpenGLRendererAPI::ClearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceCount) {
        uint32_t count = vertexArray->GetIndexBuffer()->GetCount();
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount);
    }

    void OpenGLRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::SetDepthTest(bool enabled) {
        if (enabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void OpenGLRendererAPI::BindTexture(uint32_t textureID, uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void OpenGLRendererAPI::DrawFullscreenTriangle() {
        static uint32_t emptyVAO = 0;
        if (emptyVAO == 0) {
            glGenVertexArrays(1, &emptyVAO);
        }
        glBindVertexArray(emptyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void OpenGLRendererAPI::SetDepthFunction(DepthFunction func) {
        switch (func) {
        case DepthFunction::Less:
            glDepthFunc(GL_LESS);
            break;
        case DepthFunction::LEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        }
    }

    void OpenGLRendererAPI::SetWireframeMode(bool value) {
        if (value) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void OpenGLRendererAPI::SetBlendEnabled(bool enabled) {
        if (enabled) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }

    void OpenGLRendererAPI::SetBlendFunction(BlendFunction func) {
        switch (func) {
        case BlendFunction::Alpha:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendFunction::Additive:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }
    }

    void OpenGLRendererAPI::SetDepthWriteMask(bool enabled) { glDepthMask(enabled ? GL_TRUE : GL_FALSE); }

    void OpenGLRendererAPI::SetCullFace(bool value) {
        if (value) {
            glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }
    }

    void OpenGLRendererAPI::BindCubeTexture(uint32_t rendererID, uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID); // <-- Magiczna r罂nica!
    }
} // namespace NFSEngine