#pragma once

#include <memory>
#include "Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"



namespace NFSEngine {

    class Cube {
        public:
            Cube(); 
            ~Cube() = default;

            void Draw(Shader& shader, Texture& texture);

        private:
            std::shared_ptr<VertexArray> m_VertexArray;
    };
    
}