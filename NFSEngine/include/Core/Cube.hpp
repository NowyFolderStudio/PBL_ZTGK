#pragma once

#include <memory>
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"
#include <glad/glad.h>

namespace NFSEngine {

    class Cube {
    public:
        Cube();
        ~Cube() = default;

        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Texture>& texture);

    private:
        std::shared_ptr<VertexArray> m_VertexArray;
    };

} // namespace NFSEngine