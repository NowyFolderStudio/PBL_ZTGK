#pragma once

#include <glad/glad.h>
#include <string>

namespace NFSEngine {

    class Texture {
        public:
        unsigned int ID;
        int width, height, nrChannels;
        
        Texture(const std::string& textureName);
		Texture(uint32_t width, uint32_t height);
        
		void SetData(void* data, uint32_t size);
        void Bind() const;
    };
}