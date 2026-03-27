#include "Game.hpp"
#include "Input.hpp"

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
    if (myCube) delete myCube;
}

void Game::Init() {

    myShader = new Shader("basic.vert", "basic.frag");
    myTexture = new Texture("cat.png");
    myQuad = new Quad2D();
    myText = new Text("assets/fonts/Roboto-Regular.ttf");
    textShader = new Shader("text.vert", "text.frag");  
	myCube = new Cube();

    std::cout << "DEBUG::ASSIMP: Version " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;
}

void Game::Update() {

}

void Game::Render() {
    if (!myQuad || !myCube || !myShader || !myTexture || !myText || !textShader) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    myShader->use();

    glm::mat4 model3D = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view3D = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection3D = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    myShader->setMat4("model", model3D);
    myShader->setMat4("view", view3D);
    myShader->setMat4("projection", projection3D);

    myCube->Draw(*myShader, *myTexture);

    glDisable(GL_DEPTH_TEST);

    glm::mat4 model2D = glm::mat4(1.0f);

    model2D = glm::translate(model2D, glm::vec3(-0.7f, 0.7f, 0.0f));
    model2D = glm::scale(model2D, glm::vec3(0.4f, 0.4f, 1.0f));

    myShader->setMat4("model", model2D);
    myShader->setMat4("view", glm::mat4(1.0f));
    myShader->setMat4("projection", glm::mat4(1.0f));

    myQuad->Draw(*myShader, *myTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 orthoProj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
    textShader->use();
    textShader->setMat4("projection", orthoProj);


    // Testowanie inputu klawiatury i myszy
    glm::vec3 currentTextColor = glm::vec3(1.0f, 1.0f, 0.0f);
    std::string currentText = "MEOW";

    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
        currentTextColor = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        currentText = "X: " + std::to_string((int)Input::GetMouseX())
         + " Y: " + std::to_string((int)Input::GetMouseY());
    }

    myText->Draw(*textShader, currentText, 50.0f, 50.0f, 1.0f, currentTextColor);
}