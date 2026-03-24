#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    Texture(const std::string& textureName);

    void Bind() const;
};