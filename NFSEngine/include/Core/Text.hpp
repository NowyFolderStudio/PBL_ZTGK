#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb/stb_truetype.h"

namespace NFSEngine
{

class Shader;

class Text
{
public:
    Text(const std::string& fontPath, float fontSize = 48.0f);
    ~Text();

    void Draw(std::string text, float x, float y, float scale);
    float GetTextWidth(std::string text, float scale);

private:
    GLuint VAO, VBO;
    GLuint texture;

    const int atlasWidth = 512;
    const int atlasHeight = 512;
    const int firstChar = 32;
    const int charCount = 96;

    std::vector<stbtt_packedchar> charInfo;
};
}