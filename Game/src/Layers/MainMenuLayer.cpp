#include "Layers/MainMenuLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"

MainMenuLayer::MainMenuLayer() { m_Canvas = nullptr; }

MainMenuLayer::~MainMenuLayer() {
    if (m_Canvas) delete m_Canvas;
}

void MainMenuLayer::OnAttach() {
    NFSEngine::UIRenderer::Init();
    m_Canvas = new NFSEngine::Canvas();
    BuildUI();
}

void MainMenuLayer::OnDetach() { }

void MainMenuLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (GameManager::Get().IsOptionsOpen()) return;
    if (m_Canvas) m_Canvas->Update();

    auto& input = NFSEngine::InputActionManager::Get();
    bool uiChanged = false;

    if (input.IsDown("UINavDown")) {
        m_FocusedIndex = (m_FocusedIndex + 1) % 3;
        uiChanged = true;
    } else if (input.IsDown("UINavUp")) {
        m_FocusedIndex = (m_FocusedIndex - 1 + 3) % 3;
        uiChanged = true;
    }

    if (uiChanged) BuildUI();

    if (input.IsDown("UIConfirm")) {
        if (m_FocusedIndex == 0)
            GameManager::Get().RequestStateChange(GameState::Playing);
        else if (m_FocusedIndex == 1)
            GameManager::Get().OpenOptions();
        else if (m_FocusedIndex == 2)
            NFSEngine::Application::Get().Close();
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

void MainMenuLayer::BuildUI() {
    m_Canvas->ClearUIObjects();

    const float centerX = NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f;
    const float centerY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f;

    const float leftStart = centerX - 550.0f;
    const float rightStart = centerX + 550.0f;

    glm::vec4 normalColor(0.4f, 0.4f, 0.4f, 1.0f);
    glm::vec4 focusColor(0.8f, 0.8f, 0.2f, 1.0f);

    // Background
    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(centerX, centerY, 0.1f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH - 50;
    bgParams.height = NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 50;
    bgParams.color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    // Layout
    float currentY = 200.0f;
    float elementSpacing = 170.0f;

    // Title
    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(centerX, currentY, 0.5f);
    titleParams.color = glm::vec4(1.0f);
    titleParams.text = "Out Of Tune";
    titleParams.scale = 3.0f;
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    // Title background
    NFSEngine::UI::ImageParameters titleBgParams;
    titleBgParams.position = glm::vec3(centerX, currentY, 0.4f);
    titleBgParams.width = 900.0f;
    titleBgParams.height = 200.0f;
    titleBgParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, titleBgParams);

    currentY += elementSpacing + 150;

    // Start Button
    NFSEngine::UI::ButtonParameters startParams;
    startParams.position = glm::vec3(leftStart, currentY, 0.5f);
    startParams.width = 500.0f;
    startParams.height = 100.0f;
    startParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    startParams.text = "START GAME";
    startParams.onClick = []() { GameManager::Get().RequestStateChange(GameState::Playing); };
    startParams.color = (m_FocusedIndex == 0) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, startParams);

    currentY += elementSpacing;

    // Options Button
    NFSEngine::UI::ButtonParameters optionsParams;
    optionsParams.position = glm::vec3(leftStart, currentY, 0.5f);
    optionsParams.width = 500.0f;
    optionsParams.height = 100.0f;
    optionsParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    optionsParams.text = "OPTIONS";
    optionsParams.onClick = []() { GameManager::Get().OpenOptions(); };
    optionsParams.color = (m_FocusedIndex == 1) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, optionsParams);

    currentY += elementSpacing;

    // Quit Button
    NFSEngine::UI::ButtonParameters quitParams;
    quitParams.position = glm::vec3(leftStart, currentY, 0.5f);
    quitParams.width = 500.0f;
    quitParams.height = 100.0f;
    quitParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    quitParams.text = "QUIT";
    quitParams.onClick = []() { NFSEngine::Application::Get().Close(); };
    quitParams.color = (m_FocusedIndex == 2) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, quitParams);
}