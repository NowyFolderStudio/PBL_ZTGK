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

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
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
        }
        else {
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
}