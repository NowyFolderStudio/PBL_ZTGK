#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/version.h>

#include "stb/stb_truetype.h"

#include <GLFW/glfw3.h>

Game::Game() {}

Game::~Game() {
    if (myQuad) delete myQuad;
    if (myTexture) delete myTexture;
    if (myShader) delete myShader;
}

void Game::Init() {

    myShader = new Shader("basic.vert", "basic.frag");
    myTexture = new Texture("cat.png");
    myQuad = new Quad2D();

    std::cout << "DEBUG::ASSIMP: Version " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;

    std::ifstream fontFile("assets/fonts/Roboto-Regular.ttf", std::ios::binary);
    if (fontFile) {
        std::cout << "DEBUG::STB_TRUETYPE: Font file loaded successfully!" << std::endl;
    }
    else {
        std::cout << "DEBUG::STB_TRUETYPE: Failed to find font file!" << std::endl;
    }
}

void Game::Update() {
}

void Game::Render() {
    if (myQuad && myShader && myTexture) {
        myQuad->Draw(*myShader, *myTexture);
    }

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    myShader->use();
    myShader->setMat4("model", model);
    myShader->setMat4("view", view);
    myShader->setMat4("projection", projection);
}