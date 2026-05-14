#include "Layers/MainMenuLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"

MainMenuLayer::MainMenuLayer() { m_Canvas = nullptr; }

MainMenuLayer::~MainMenuLayer() {
    if (m_Canvas) delete m_Canvas;
}

void MainMenuLayer::OnAttach() {
    float virtualWidth = 1920.0f;
    float virtualHeight = 1080.0f;

    NFSEngine::UIRenderer::Init();
    NFSEngine::UIRenderer::SetProjection(virtualWidth, virtualHeight);
    m_Canvas = new NFSEngine::Canvas();

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(virtualWidth / 2.0f, virtualHeight / 2.0f, 0.1f);
    bgParams.width = virtualWidth;
    bgParams.height = virtualHeight;
    bgParams.color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(virtualWidth / 2.0f, 350.0f, 0.5f);
    titleParams.text = "NFSEngine GAME";
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    NFSEngine::UI::ButtonParameters startParams;
    startParams.position = glm::vec3(virtualWidth / 2.0f, 500, 0.5f);
    startParams.width = 400.0f;
    startParams.height = 100.0f;
    startParams.color = glm::vec4(0.2f, 0.6f, 0.2f, 1.0f);
    startParams.text = "START GAME";
    startParams.onClick = []() { GameManager::Get().RequestStateChange(GameState::Playing); };
    NFSEngine::UI::Button(*m_Canvas, startParams);

    NFSEngine::UI::ButtonParameters quitParams;
    quitParams.position = glm::vec3(virtualWidth / 2.0f, 800.0f, 0.5f);
    quitParams.width = 400.0f;
    quitParams.height = 100.0f;
    quitParams.color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);
    quitParams.text = "QUIT";
    quitParams.onClick = []() { NFSEngine::Application::Get().Close(); };
    NFSEngine::UI::Button(*m_Canvas, quitParams);
}

void MainMenuLayer::OnDetach() { }

void MainMenuLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Canvas) {
        m_Canvas->Update();
    }
}

void MainMenuLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_Canvas) {
        m_Canvas->Draw();
    }
    NFSEngine::UIRenderer::End();
}

void MainMenuLayer::OnEvent(NFSEngine::Event& e) { }