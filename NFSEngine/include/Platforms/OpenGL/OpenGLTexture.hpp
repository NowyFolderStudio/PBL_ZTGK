#pragma once
#include "Renderer/Texture.hpp"
#include <glad/glad.h>

namespace NFSEngine {
    struct GLTextureParameters {
        GLenum WrapS = GL_REPEAT;
        GLenum WrapT = GL_REPEAT;
        GLenum MinFilter = GL_LINEAR_MIPMAP_LINEAR;
        GLenum MagFilter = GL_LINEAR;
        bool GenerateMipmaps = true;
    };

    class OpenGLTexture : public Texture {
    public:
        OpenGLTexture(const std::string& path, GLTextureParameters parameters = GLTextureParameters());
        OpenGLTexture(uint32_t width, uint32_t height);
        virtual ~OpenGLTexture();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override;
        virtual void Bind(uint32_t slot = 0) const override;

    private:
        std::string m_Path;
        GLTextureParameters m_Parameters;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };
} // namespace NFSEngine