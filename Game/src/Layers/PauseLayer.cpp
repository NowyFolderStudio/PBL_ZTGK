#include "Layers/PauseLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"
#include "Core/Input.hpp" // zakladam, ze masz tu KeyCodes

PauseLayer::PauseLayer() { m_Canvas = nullptr; }

PauseLayer::~PauseLayer() {
    if (m_Canvas) delete m_Canvas;
}

void PauseLayer::OnAttach() {
    m_Canvas = new NFSEngine::Canvas();

    const float centerX = NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f;
    const float centerY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f;

    const float leftStart = centerX - 550.0f;
    const float rightStart = centerX + 550.0f;

    // Background
    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(centerX, centerY, 0.1f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH - 200.0f;
    bgParams.height = NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 200.0f;
    bgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.7f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    // Layout
    float currentY = 200.0f;
    float elementSpacing = 170.0f;

    // Title
    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(centerX, currentY, 0.5f);
    titleParams.text = "PAUSE MENU";
    titleParams.scale = 2.0f;
    titleParams.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    currentY += elementSpacing;

    // Resume Button
    NFSEngine::UI::ButtonParameters resumeParams;
    resumeParams.position = glm::vec3(centerX, currentY, 0.5f);
    resumeParams.width = 500.0f;
    resumeParams.height = 100.0f;
    resumeParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    resumeParams.text = "RESUME";
    resumeParams.textColor = glm::vec4(1.0f);
    resumeParams.onClick = []() { GameManager::Get().TogglePause(); };
    NFSEngine::UI::Button(*m_Canvas, resumeParams);

    currentY += elementSpacing;

    // Options Button
    NFSEngine::UI::ButtonParameters optionsParams;
    optionsParams.position = glm::vec3(centerX, currentY, 0.5f);
    optionsParams.width = 500.0f;
    optionsParams.height = 100.0f;
    optionsParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    optionsParams.text = "OPTIONS";
    optionsParams.textColor = glm::vec4(1.0f);
    optionsParams.onClick = []() { GameManager::Get().OpenOptions(); };
    NFSEngine::UI::Button(*m_Canvas, optionsParams);

    currentY += elementSpacing;
    currentY += 100.0f;

    // Quit Button
    NFSEngine::UI::ButtonParameters quitParams;
    quitParams.position = glm::vec3(centerX, currentY, 0.5f);
    quitParams.width = 500.0f;
    quitParams.height = 100.0f;
    quitParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    quitParams.text = "QUIT TO MENU";
    quitParams.textColor = glm::vec4(1.0f);
    quitParams.onClick = []() { GameManager::Get().RequestStateChange(GameState::MainMenu); };
    NFSEngine::UI::Button(*m_Canvas, quitParams);
}

void PauseLayer::OnDetach() { }

void PauseLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (GameManager::Get().IsOptionsOpen()) {
        return;
    }

    if (m_Canvas) {
        m_Canvas->Update();
    }
}

void PauseLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_Canvas) {
        m_Canvas->Draw();
    }
    NFSEngine::UIRenderer::End();
}

void PauseLayer::OnEvent(NFSEngine::Event& e) {
    NFSEngine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<NFSEngine::WindowResizeEvent>([](NFSEngine::WindowResizeEvent& ev) {
        NFSEngine::Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());
        return false;
    });

    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;
        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }
    }
}