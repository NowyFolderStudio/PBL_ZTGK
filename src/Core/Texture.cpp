#include "Core/Texture.hpp"
#include <iostream>

#include "stb/stb_image.h" 

namespace NFSEngine {

    Texture::Texture(const std::string& textureName) {
        std::string fullPath = "assets/textures/" + textureName;
        
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_set_flip_vertically_on_load(true);
        
        unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
        
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE: " << fullPath << std::endl;
            std::cout << "POWOD BLEDY STB: " << stbi_failure_reason() << std::endl;
        }
        
        stbi_image_free(data);
    }
    
    void Texture::Bind() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
}