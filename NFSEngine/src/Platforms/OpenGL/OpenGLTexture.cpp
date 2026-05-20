#include "Platforms/OpenGL/OpenGLTexture.hpp"

#include "stb/stb_image.h"
#include <glad/glad.h>
#include "nfspch.h"

namespace NFSEngine {

    static GLenum NFSTextureWrapToGL(TextureWrap wrap) {
        switch (wrap) {
        case TextureWrap::Repeat: return GL_REPEAT;
        case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
        }
        return GL_REPEAT;
    }

    static GLenum NFSTextureFilterToGL(TextureFilter filter, bool isMinFilter, bool mipmaps) {
        if (filter == TextureFilter::Linear) {
            return (isMinFilter && mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        }
        else {
            return (isMinFilter && mipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        }
    }

    OpenGLTexture::OpenGLTexture(const std::string& path, const TextureParameters& parameters)
        : m_Path(path)
        , m_Parameters(parameters) {

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);

        if (stbi_is_hdr(path.c_str())) {
            float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

            if (data) {
                m_Width = width;
                m_Height = height;

                GLenum internalFormat = 0, dataFormat = 0;
                if (channels == 3) {
                    internalFormat = GL_RGB16F;
                    dataFormat = GL_RGB;
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                }
                else if (channels == 4) {
                    internalFormat = GL_RGBA16F;
                    dataFormat = GL_RGBA;
                }

                m_InternalFormat = internalFormat;
                m_DataFormat = dataFormat;

                glGenTextures(1, &m_RendererID);
                glBindTexture(GL_TEXTURE_2D, m_RendererID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, NFSTextureWrapToGL(parameters.WrapS));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, NFSTextureWrapToGL(parameters.WrapT));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NFSTextureFilterToGL(parameters.MinFilter, true, parameters.GenerateMipmaps));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, NFSTextureFilterToGL(parameters.MagFilter, false, false));

                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_FLOAT, data);
                if (parameters.GenerateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                stbi_image_free(data);
            }
            else {
                NFS_CORE_ERROR("ERROR::TEXTURE::FAILED_TO_LOAD_HDR: {}", path);
                NFS_CORE_ERROR("STB REASON: {}", stbi_failure_reason());
            }
        }
        else {
            stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

            if (data) {
                m_Width = width;
                m_Height = height;

                GLenum internalFormat = 0, dataFormat = 0;
                if (channels == 1) {
                    internalFormat = GL_R8;
                    dataFormat = GL_RED;
                }
                else if (channels == 3) {
                    internalFormat = parameters.sRGB ? GL_SRGB8 : GL_RGB8;
                    dataFormat = GL_RGB;
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                }
                else if (channels == 4) {
                    internalFormat = parameters.sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                    dataFormat = GL_RGBA;
                }

                m_InternalFormat = internalFormat;
                m_DataFormat = dataFormat;

                glGenTextures(1, &m_RendererID);
                glBindTexture(GL_TEXTURE_2D, m_RendererID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, NFSTextureWrapToGL(parameters.WrapS));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, NFSTextureWrapToGL(parameters.WrapT));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NFSTextureFilterToGL(parameters.MinFilter, true, parameters.GenerateMipmaps));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, NFSTextureFilterToGL(parameters.MagFilter, false, false));

                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
                if (parameters.GenerateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                stbi_image_free(data);
            }
            else {
                NFS_CORE_ERROR("ERROR::TEXTURE::FAILED_TO_LOAD: {}", path);
                NFS_CORE_ERROR("STB REASON: {}", stbi_failure_reason());
            }
        }
    }

    OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, const TextureParameters& parameters)
        : m_Width(width)
        , m_Height(height) {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, NFSTextureWrapToGL(parameters.WrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, NFSTextureWrapToGL(parameters.WrapT));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NFSTextureFilterToGL(parameters.MinFilter, true, parameters.GenerateMipmaps));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, NFSTextureFilterToGL(parameters.MagFilter, false, false));

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
    }

    OpenGLTexture::~OpenGLTexture() { glDeleteTextures(1, &m_RendererID); }

    void OpenGLTexture::SetData(void* data, uint32_t size) {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        if (size != m_Width * m_Height * bpp) {
            NFS_CORE_ERROR("ERROR::TEXTURE::SET_DATA: Data size mismatch!");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }
} // namespace NFSEngine