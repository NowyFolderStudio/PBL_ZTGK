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
        uint32_t Channels = 4;
    };

    class OpenGLTexture : public Texture {
    public:
        OpenGLTexture(const std::string& path, const TextureParameters& parameters);
        OpenGLTexture(uint32_t width, uint32_t height, const TextureParameters& parameters);
        virtual ~OpenGLTexture();

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetRendererID() const override { return m_RendererID; }

        void SetData(void* data, uint32_t size) override;
        void UpdateData(const void* data, uint32_t width, uint32_t height) override;
        void Bind(uint32_t slot = 0) const override;

    private:
        std::string m_Path;
        TextureParameters m_Parameters;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };
} // namespace NFSEngine