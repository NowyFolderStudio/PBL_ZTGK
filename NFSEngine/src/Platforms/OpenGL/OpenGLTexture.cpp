#include "Platforms/OpenGL/OpenGLTexture.hpp"

#include "stb/stb_image.h"
#include <glad/glad.h>
#include "nfspch.h"

namespace NFSEngine {
    OpenGLTexture::OpenGLTexture(const std::string& path, GLTextureParameters parameters)
        : m_Path(path)
        , m_Parameters(parameters) {

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);

        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data) {
            m_Width = width;
            m_Height = height;

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4) {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            } else if (channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            }

            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;

            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters.WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters.WrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters.MinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters.MagFilter);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            if (parameters.GenerateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            stbi_image_free(data);
        } else {
            NFS_CORE_ERROR("ERROR::TEXTURE::FAILED_TO_LOAD: ", path);
            NFS_CORE_ERROR("STB REASON:  ", stbi_failure_reason());
        }
    }

    OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height)
        : m_Width(width)
        , m_Height(height) {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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