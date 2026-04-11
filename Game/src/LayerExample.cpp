#include "LayerExample.hpp"
#include "Core/Application.hpp"
#include "Core/Components/CubeMesh.hpp"
#include "Core/Components/Transform.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "GLFW/glfw3.h"
#include <cstddef>
#include <memory>
#include "Components/CubeControl.hpp"
#include "Renderer/Renderer.hpp"

#include "Events/ApplicationEvent.hpp"

LayerExample::LayerExample()
{
    myShader = nullptr;
    myTexture = nullptr;
    myCube = nullptr;
    movingCube = nullptr;
    scene = nullptr;
}

LayerExample::~LayerExample()
{
    if (myCube) delete myCube;
}

void LayerExample::OnAttach()
{
    Init();
    scene = std::make_unique<NFSEngine::Scene>();
    movingCube = scene->CreateGameObject("movingCube");
    movingCube->AddComponent<NFSEngine::Transform>();

    auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");

    movingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    movingCube->AddComponent<CubeControl>();
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime)
{
    scene->OnUpdate(deltaTime);
    Update();
    Render();
}

void LayerExample::Init()
{
    myShader = NFSEngine::Shader::Create("MainShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    myTexture = NFSEngine::Texture::Create("assets/textures/cat.png");
    myCube = new NFSEngine::Cube();
}

void LayerExample::Update() { }

void LayerExample::Render()
{

    // 1. Czyszczenie ekranu za pomoc¹ naszego interfejsu (¿adnego glClear!)
    NFSEngine::Renderer::GetAPI().SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
    NFSEngine::Renderer::GetAPI().Clear();

    // 2. Przygotowanie kamery
    glm::mat4 view3D = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection3D = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    // 3. Rozpoczêcie sceny (przekazujemy kamerê do Renderera)
    NFSEngine::Renderer::BeginScene(view3D, projection3D);

    // 4. Renderowanie sceny
    // To wywo³a Scene -> GameObject -> CubeMesh, które dodadz¹ siê do kolejki Renderera!
    if (scene)
    {
        scene->OnRender();
    }

    // (Jeœli Twój stary obiekt 'myCube' u¿ywa jeszcze starych metod,
    // przerób go tak samo jak movingCube, ¿eby korzysta³ ze Sceny i ECS)

    // 5. Zakoñczenie sceny (WYS£ANIE WSZYSTKIEGO DO KARTY GRAFICZNEJ)
    NFSEngine::Renderer::EndScene();

    /*glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!myCube || !myShader || !myTexture) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    myShader->Bind();

    glm::mat4 model3D = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view3D = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection3D = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    myShader->SetMat4("model", model3D);
    myShader->SetMat4("view", view3D);
    myShader->SetMat4("projection", projection3D);

    myCube->Draw(myShader, myTexture);
    movingCube->Render();

    glDisable(GL_DEPTH_TEST);

    glm::mat4 model2D = glm::mat4(1.0f);

    model2D = glm::translate(model2D, glm::vec3(-0.7f, 0.7f, 0.0f));
    model2D = glm::scale(model2D, glm::vec3(0.4f, 0.4f, 1.0f));

    myShader->SetMat4("model", model2D);
    myShader->SetMat4("view", glm::mat4(1.0f));
    myShader->SetMat4("projection", glm::mat4(1.0f));*/
}

void LayerExample::OnEvent(NFSEngine::Event& e) { }