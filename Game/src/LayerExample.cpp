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

#include "Events/ApplicationEvent.hpp"

    LayerExample::LayerExample() {
        myShader = nullptr;
        myTexture = nullptr;
        myCube = nullptr;
        canvas = nullptr;
        myText = nullptr;
        textShader = nullptr;
        movingCube = nullptr;
        scene = nullptr;
    }
    
    LayerExample::~LayerExample() {
        if (myText) delete myText;
        if (myCube) delete myCube;
        if (movingCube) delete movingCube;

		delete canvas;
        NFSEngine::UIRenderer::Shutdown();
    }

    void LayerExample::OnAttach() {
        Init();
        scene = std::make_unique<NFSEngine::Scene>();
        movingCube = scene->CreateGameObject("movingCube");
        movingCube->AddComponent<NFSEngine::Transform>();
        
        auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
        auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");

        movingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);
        movingCube->AddComponent<CubeControl>();
    }

    void LayerExample::OnDetach() {

    }

    void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
        scene->OnUpdate(deltaTime);
        Update();
        Render();
    }
    
    void LayerExample::Init() {
        myShader = NFSEngine::Shader::Create("MainShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
        myTexture = NFSEngine::Texture::Create("assets/textures/cat.png");
        myText = new NFSEngine::Text("assets/fonts/Roboto-Regular.ttf");
        textShader = NFSEngine::Shader::Create("TextShader", "assets/shaders/text.vert", "assets/shaders/text.frag");
        myCube = new NFSEngine::Cube();

		float windowWidth = (float)NFSEngine::Application::Get().GetConfig().WindowWidth;
		float windowHeight = (float)NFSEngine::Application::Get().GetConfig().WindowHeight;

        // przykladowe ui - styl inicjowania tego moze sie jeszcze zmienic bo nie jest idealnie intuicyjny ale na razie to nie jest priorytet raczej

		NFSEngine::UIRenderer::Init();
		NFSEngine::UIRenderer::SetProjection(windowWidth, windowHeight); // tu musi byc przekazywana aktualna szerokosc i wysokosc okna, zeby UI bylo poprawnie skalowane i pozycjonowane
        canvas = new NFSEngine::Canvas();

		NFSEngine::UI::ButtonParameters buttonParams;
		buttonParams.position = glm::vec3(500, 300, 2.0f);
		buttonParams.text = "PLAY";
        buttonParams.onClick = []() {
            std::cout << "Button clicked!" << std::endl;
			};
		NFSEngine::UI::Button(*canvas, buttonParams);

		NFSEngine::UI::ImageParameters imageParams;
		imageParams.position = glm::vec3(500, 300, 1.0f);
		imageParams.width = 950;
		imageParams.height = 550;
		imageParams.color = glm::vec4(0.0f, 0.5f, 1.0f, 0.3f);
		NFSEngine::UI::Image(*canvas, imageParams);

		NFSEngine::UI::CheckboxParameters checkboxParams;
		checkboxParams.position = glm::vec3(200, 300, 0.0f);
        checkboxParams.onToggle = [](bool checked) {
            std::cout << "Checkbox toggled: " << (checked ? "Checked" : "Unchecked") << std::endl;
			};
		NFSEngine::UI::Checkbox(*canvas, checkboxParams);
        // koniec ui
    }
    
    void LayerExample::Update() {
		canvas->Update();
    }
    
    void LayerExample::Render() {
        glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!myCube || !myShader || !myTexture || !myText || !textShader) return;

        static int pressedCount = 0;
        static int releasedCount = 0;

        if (NFSEngine::Input::IsKeyDown(NFSEngine::Key::Space) || NFSEngine::Input::IsMouseButtonDown(NFSEngine::Mouse::ButtonLeft)) {
            pressedCount++;
        }

        if (NFSEngine::Input::IsKeyUp(NFSEngine::Key::Space) || NFSEngine::Input::IsMouseButtonUp(NFSEngine::Mouse::ButtonLeft)) {
            releasedCount++;
        }

        std::string counterText1 = "C: " + std::to_string(pressedCount);
        std::string counterText2 = "R: " + std::to_string(releasedCount);

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
        myShader->SetMat4("projection", glm::mat4(1.0f));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		canvas->Draw();

        glm::mat4 orthoProj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        textShader->Bind();
        textShader->SetMat4("projection", orthoProj);

        glm::vec3 currentTextColor = glm::vec3(1.0f, 1.0f, 0.0f);
        std::string currentText = "MEOW";

        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::Space)) {
            currentTextColor = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (NFSEngine::Input::IsMouseButtonPressed(NFSEngine::Mouse::ButtonLeft)) {
            currentText = "X: " + std::to_string((int)NFSEngine::Input::GetMouseX())
            + " Y: " + std::to_string((int)NFSEngine::Input::GetMouseY());
        }


        float uiX = 950.0f;
    }

    void LayerExample::OnEvent(NFSEngine::Event& e) { // resize okna raczej nie powinien byc w warstwie
        NFSEngine::EventDispatcher dispatcher(e);

        dispatcher.Dispatch<NFSEngine::WindowResizeEvent>([this](NFSEngine::WindowResizeEvent& event) { // to zostanie przeniesione ale jeszcze nie teraz
            NFSEngine::UIRenderer::SetProjection((float)event.GetWidth(), (float)event.GetHeight());
            return false;
            });
    }