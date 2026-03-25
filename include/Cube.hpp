#pragma once
#include <glad/glad.h>
#include "Shader.hpp"
#include "Texture.hpp"

class Cube {
private:
    unsigned int VAO, VBO;
public:
    Cube();
    ~Cube();
    void Draw(Shader& shader, Texture& texture);
};