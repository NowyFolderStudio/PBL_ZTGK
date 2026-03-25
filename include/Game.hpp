#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "Quad2D.hpp"
#include "Text.hpp"
#include "Cube.hpp"

class Game {
public:
    Game();
    ~Game();

    void Init();
    void Update();
    void Render();

private:
    Shader* myShader = nullptr;
    Texture* myTexture = nullptr;
    Quad2D* myQuad = nullptr;
    Text* myText = nullptr;
    Shader* textShader = nullptr;
	Cube* myCube = nullptr;
};