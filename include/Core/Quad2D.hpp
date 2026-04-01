#pragma once
#include <glad/glad.h>
#include "Shader.hpp"
#include "Texture.hpp"

namespace NFSEngine {

    class Quad2D {
        public:
        Quad2D();
        ~Quad2D();
        
        void Draw();
        
        private:
        unsigned int VAO, VBO, EBO;
        void Setup();
    };

}