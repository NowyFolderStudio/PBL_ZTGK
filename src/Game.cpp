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
    if (myText) delete myText;
    if (textShader) delete textShader;
}

void Game::Init() {

    myShader = new Shader("basic.vert", "basic.frag");
    myTexture = new Texture("cat.png");
    myQuad = new Quad2D();
    myText = new Text("assets/fonts/Roboto-Regular.ttf");
    textShader = new Shader("text.vert", "text.frag");

    std::cout << "DEBUG::ASSIMP: Version " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;
}

void Game::Update() {
}

void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    
    myShader->use();
    myShader->setMat4("model", model);
    myShader->setMat4("view", view);
    myShader->setMat4("projection", projection);
    
    if (myQuad && myShader && myTexture) {
        myQuad->Draw(*myShader, *myTexture);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 orthoProj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
    textShader->use();
    textShader->setMat4("projection", orthoProj);

    myText->Draw(*textShader, "MEOW", 650.0f, 150.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
}