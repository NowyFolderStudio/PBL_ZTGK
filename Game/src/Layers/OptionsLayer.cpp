#include "Layers/OptionsLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"
#include "Core/Input.hpp" // zakladam, ze masz tu KeyCodes

OptionsLayer::OptionsLayer() { m_Canvas = nullptr; }

OptionsLayer::~OptionsLayer() {
    if (m_Canvas) delete m_Canvas;
}

void OptionsLayer::OnAttach() {
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
    titleParams.text = "OPTIONS";
    titleParams.scale = 2.0f;
    titleParams.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    // Back button
    NFSEngine::UI::ButtonParameters backParams;
    backParams.position = glm::vec3(centerX, 800.0f, 0.5f);
    backParams.text = "BACK";
    backParams.onClick = []() { GameManager::Get().CloseOptions(); };
    NFSEngine::UI::Button(*m_Canvas, backParams);
}

void OptionsLayer::OnDetach() { }

void OptionsLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Canvas) {
        m_Canvas->Update();
    }
}

void OptionsLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_Canvas) {
        m_Canvas->Draw();
    }
    NFSEngine::UIRenderer::End();
}

void OptionsLayer::OnEvent(NFSEngine::Event& e) {
    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;

        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().CloseOptions();
            e.Handled = true;
        }
    }
}