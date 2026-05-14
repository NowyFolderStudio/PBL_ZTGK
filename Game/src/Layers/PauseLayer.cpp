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
    float virtualWidth = 1920.0f;
    float virtualHeight = 1080.0f;

    NFSEngine::UIRenderer::SetProjection(virtualWidth, virtualHeight);
    m_Canvas = new NFSEngine::Canvas();

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(virtualWidth / 2.0f, virtualHeight / 2.0f, 0.1f);
    bgParams.width = virtualWidth;
    bgParams.height = virtualHeight;
    bgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.7f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(virtualWidth / 2.0f, 450.0f, 0.5f);
    titleParams.text = "PAUZA";
    titleParams.scale = 2.0f;
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    NFSEngine::UI::ButtonParameters resumeParams;
    resumeParams.position = glm::vec3(virtualWidth / 2.0f, 600.0f, 0.5f);
    resumeParams.width = 400.0f;
    resumeParams.height = 100.0f;
    resumeParams.color = glm::vec4(0.2f, 0.6f, 0.2f, 1.0f);
    resumeParams.text = "WZNOW GRE";
    resumeParams.onClick = []() { GameManager::Get().TogglePause(); };
    NFSEngine::UI::Button(*m_Canvas, resumeParams);

    NFSEngine::UI::ButtonParameters quitParams;
    quitParams.position = glm::vec3(virtualWidth / 2.0f, 800.0f, 0.5f);
    quitParams.width = 400.0f;
    quitParams.height = 100.0f;
    quitParams.color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);
    quitParams.text = "WYJDZ DO MENU";
    quitParams.onClick = []() { GameManager::Get().RequestStateChange(GameState::MainMenu); };
    NFSEngine::UI::Button(*m_Canvas, quitParams);
}

void PauseLayer::OnDetach() { }

void PauseLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
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
    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;
        if (keyEvent.GetKeyCode() == NFSEngine::Key::Q) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }
    }
}