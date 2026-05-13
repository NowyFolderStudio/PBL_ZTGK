#include "UILayer.hpp"
#include "Core/DeltaTime.hpp"
#include "GameStateView.hpp"

UILayer::UILayer() { m_Canvas = nullptr; }

UILayer::~UILayer() {
    if (m_Canvas) delete m_Canvas;
    NFSEngine::UIRenderer::Shutdown();
}

void UILayer::OnAttach() { Init(); }

void UILayer::OnDetach() {}

void UILayer::Init() {
    constexpr int k_MaxLives = 3;
    float virtualWidth = 1920.0f;
    float virtualHeight = 1080.0f;

    NFSEngine::UIRenderer::Init();
    NFSEngine::UIRenderer::SetProjection(virtualWidth, virtualHeight);
    m_Canvas = new NFSEngine::Canvas();

    NFSEngine::UI::LabelParameters labelParams;
    labelParams.position = glm::vec3(1500, 100, 2.0f);
    labelParams.text = "SCORE: 0";
    m_ScoreLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(455, 110, 0.5f);
    bgParams.width = 400;
    bgParams.height = 70;
    bgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    NFSEngine::UI::ImageParameters barParams;
    barParams.position = glm::vec3(255, 110, 0.5f);
    barParams.width = 400;
    barParams.height = 70;
    barParams.color = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
    m_AnimatedBar = &NFSEngine::UI::Image(*m_Canvas, barParams);
    m_AnimatedBar->Transform.Pivot = glm::vec2(0.0f, 0.5f);

    // Hearths
    NFSEngine::UI::ImageParameters heartBgParams;
    heartBgParams.position = glm::vec3(175, 50, 0.8f);
    heartBgParams.width = 310;
    heartBgParams.height = 70;
    heartBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.45f);
    NFSEngine::UI::Image(*m_Canvas, heartBgParams);

    const float heartSize = 60.0f;
    const float heartSpacing = 90.0f;
    const float heartStartX = 50.0f;
    const float heartY = 50.0f;

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

void UILayer::SetGameState(std::shared_ptr<GameStateView> view) {
    m_GameState = std::move(view);
}

void UILayer::SyncFromGameState() {
    if (!m_GameState) return;

    if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>()) {
        m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString =
            "SCORE: " + std::to_string(m_GameState->data.score);
    }

    UpdateHeartVisuals();
}

void UILayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    m_AnimationTime += deltaTime.GetSeconds();
    if (m_AnimatedBar) {

        float animatedWidth = 200.0f + std::sin(m_AnimationTime * 3.0f) * 150.0f;
        m_AnimatedBar->Transform.Width = animatedWidth;
    }

    if (m_BgShape1 && m_BgShape2) {
        m_BgShape1->Transform.Position.x = 960.0f + std::sin(m_AnimationTime * 0.4f) * 400.0f;
        m_BgShape1->Transform.Position.y = 540.0f + std::cos(m_AnimationTime * 0.3f) * 200.0f;

        m_BgShape2->Transform.Position.x = 960.0f + std::cos(m_AnimationTime * 0.5f) * 300.0f;
        m_BgShape2->Transform.Position.y = 540.0f + std::sin(m_AnimationTime * 0.6f) * 300.0f;
    }

    SyncFromGameState();
    Update();
}

void UILayer::OnRender() { Render(); }

void UILayer::Update() { m_Canvas->Update(); }

void UILayer::Render() {
    NFSEngine::UIRenderer::Begin();
    m_Canvas->Draw();
    NFSEngine::UIRenderer::End();
}

void UILayer::OnEvent(NFSEngine::Event& e) {}

void UILayer::UpdateHeartVisuals() {
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
