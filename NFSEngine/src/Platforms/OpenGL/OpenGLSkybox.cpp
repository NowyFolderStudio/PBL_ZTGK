#include "Platforms/OpenGL/OpenGLSkybox.hpp"
#include "stb/stb_image.h"
#include <iostream>

namespace NFSEngine {
    OpenGLSkybox::OpenGLSkybox(const std::vector<std::string>& facePaths) {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);

        for (uint32_t i = 0; i < facePaths.size(); i++) {
            unsigned char* data = stbi_load(facePaths[i].c_str(), &width, &height, &channels, 0);
            if (data) {
                GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                std::cout << "Skybox Texture failed to load at path: " << facePaths[i] << std::endl;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLSkybox::~OpenGLSkybox() {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLSkybox::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
    }

    std::shared_ptr<Skybox> Skybox::Create(const std::vector<std::string>& facePaths) {
        return std::make_shared<OpenGLSkybox>(facePaths);
    }
}