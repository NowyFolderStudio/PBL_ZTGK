#include "Platforms/OpenGL/OpenGLVertexArray.hpp"
#include <glad/glad.h>

namespace NFSEngine {

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        case ShaderDataType::one:
            return GL_NONE;
        }

        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() { glGenVertexArrays(1, &m_RendererID); }

    void OpenGLVertexArray::Bind() const { glBindVertexArray(m_RendererID); }

    void OpenGLVertexArray::Unbind() const { glBindVertexArray(0); }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(m_VertexBufferIndex);

            if (element.Type == ShaderDataType::Int || element.Type == ShaderDataType::Int2
                || element.Type == ShaderDataType::Int3 || element.Type == ShaderDataType::Int4) {
                glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                       ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(),
                                       (const void*)(uintptr_t)element.Offset);
            } else {
                glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                      ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE,
                                      layout.GetStride(), (const void*)(uintptr_t)element.Offset);
            }
            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::AddInstancedVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(m_VertexBufferIndex);

            if (element.Type == ShaderDataType::Int || element.Type == ShaderDataType::Int2
                || element.Type == ShaderDataType::Int3 || element.Type == ShaderDataType::Int4) {
                glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                       ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(),
                                       (const void*)(uintptr_t)element.Offset);
            } else {
                glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                      ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE,
                                      layout.GetStride(), (const void*)(uintptr_t)element.Offset);
            }

            glVertexAttribDivisor(m_VertexBufferIndex, 1);

            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }
} // namespace NFSEngine