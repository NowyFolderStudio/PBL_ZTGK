#include "Layers/HUDLayer.hpp"
#include "Core/DeltaTime.hpp"
#include "GameStateView.hpp"

HUDLayer::HUDLayer() { m_Canvas = nullptr; }

HUDLayer::~HUDLayer() {
    if (m_Canvas) delete m_Canvas;
    NFSEngine::UIRenderer::Shutdown();
}

void HUDLayer::OnAttach() {
    m_Canvas = new NFSEngine::Canvas();

    InitScoreUI();
    InitHeartsUI();
    InitAuraUI();
}

void HUDLayer::OnDetach() { }

void HUDLayer::SetGameState(std::shared_ptr<GameStateView> view) { m_GameState = std::move(view); }

void HUDLayer::SyncFromGameState() {
    if (!m_GameState) return;

    if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>()) {
        m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString = "SCORE: " + std::to_string(m_GameState->data.score);
    }

    UpdateHeartVisuals();
}

void HUDLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    SyncFromGameState();
    AnimateElements(deltaTime);
    m_Canvas->Update();
}

void HUDLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    m_Canvas->Draw();
    NFSEngine::UIRenderer::End();
}

void HUDLayer::OnEvent(NFSEngine::Event& e) { }

void HUDLayer::UpdateHeartVisuals() {
    if (!m_GameState) return;
    int lives = m_GameState->data.lives;
    for (size_t i = 0; i < m_Hearts.size(); ++i) {
        if (!m_Hearts[i]) continue;

        auto* img = m_Hearts[i]->GetComponent<NFSEngine::ImageComponent>();
        if (!img) continue;

        if (static_cast<int>(i) < lives) {
            img->Color = glm::vec4(0.9f, 0.1f, 0.15f, 1.0f);
        } else {
            img->Color = glm::vec4(0.3f, 0.3f, 0.3f, 0.4f);
        }
    }
}

void HUDLayer::InitScoreUI() {
    NFSEngine::UI::LabelParameters labelParams;
    labelParams.position = glm::vec3(1500, 100, 2.0f);
    labelParams.text = "SCORE: 0";
    labelParams.color = glm::vec4(1.0f);
    m_ScoreLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);

    NFSEngine::UI::ImageParameters scoreBgParams;
    scoreBgParams.position = glm::vec3(1500, 100, 1.0f);
    scoreBgParams.width = 400;
    scoreBgParams.height = 100;
    scoreBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    NFSEngine::UI::Image(*m_Canvas, scoreBgParams);
}

void HUDLayer::InitHeartsUI() {
    constexpr int k_MaxLives = 3; // XD?
    NFSEngine::UI::ImageParameters heartBgParams;
    heartBgParams.position = glm::vec3(300, 100, 0.8f);
    heartBgParams.width = 400;
    heartBgParams.height = 100;
    heartBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.45f);
    NFSEngine::UI::Image(*m_Canvas, heartBgParams);

    const float heartSize = 60.0f;
    const float heartSpacing = 120.0f;
    const float heartStartX = 195.0f;
    const float heartY = 100.0f;

    m_Hearts.reserve(k_MaxLives);
    for (int i = 0; i < k_MaxLives; ++i) {
        NFSEngine::UI::ImageParameters heartParams;
        heartParams.position = glm::vec3(heartStartX + i * heartSpacing, heartY, 1.0f);
        heartParams.width = heartSize;
        heartParams.height = heartSize;
        heartParams.color = glm::vec4(0.9f, 0.1f, 0.15f, 1.0f);
        m_Hearts.push_back(&NFSEngine::UI::Image(*m_Canvas, heartParams));
    }
}

void HUDLayer::InitAuraUI() {
    // pass
}

void HUDLayer::AnimateElements(NFSEngine::DeltaTime deltaTime) {
    // pass
}