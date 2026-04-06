#include "LayerExample.hpp"
#include "Core/Application.hpp"
#include "GLFW/glfw3.h"


    LayerExample::LayerExample() {
        myShader = nullptr;
        myTexture = nullptr;
        myCube = nullptr;
        canvas = nullptr;
        myText = nullptr;
        textShader = nullptr;
    }
    
    LayerExample::~LayerExample() {
        if (myTexture) delete myTexture;
        if (myShader) delete myShader;
        if (myText) delete myText;
        if (textShader) delete textShader;
        if (myCube) delete myCube;

		delete canvas;
        NFSEngine::UIRenderer::Shutdown();
    }

    void LayerExample::OnAttach() {
        Init();
    }

    void LayerExample::OnDetach() {

    }

    void LayerExample::OnUpdate() {
        Update();
        Render();
    }
    
    void LayerExample::Init() {
        myShader = new NFSEngine::Shader("basic.vert", "basic.frag");
        myTexture = new NFSEngine::Texture("cat.png");
        myText = new NFSEngine::Text("assets/fonts/Roboto-Regular.ttf");
        textShader = new NFSEngine::Shader("text.vert", "text.frag");  
        myCube = new NFSEngine::Cube();

		NFSEngine::UIRenderer::Init();
		NFSEngine::UIRenderer::SetProjection(1280.0f, 720.0f);
        canvas = new NFSEngine::Canvas();

        NFSEngine::UI::Image(*canvas, glm::vec3(640.0f, 360.0f, 1.0f), 1240.0f, 680.0f, glm::vec4(0.0f, 0.5f, 1.0f, 0.3f));
        NFSEngine::UI::Button(*canvas, glm::vec3(640.0f, 360.0f, 2.0f), 150.0f, 50.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), []() {
            std::cout << "Button clicked!" << std::endl;
			});
        NFSEngine::UI::Label(*canvas, glm::vec3(600.0f, 370.0f, 3.0f), "PLAY", nullptr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		canvas->Draw();

        glm::mat4 orthoProj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
        textShader->use();
        textShader->setMat4("projection", orthoProj);

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