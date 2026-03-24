#include "Game.hpp"

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

}

void Game::Update() {
}

void Game::Render() {
    if (myQuad && myShader && myTexture) {
        myQuad->Draw(*myShader, *myTexture);
    }
}