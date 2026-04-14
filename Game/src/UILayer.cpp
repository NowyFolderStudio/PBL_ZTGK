#include "UILayer.hpp"
#include "Core/Application.hpp"
#include "Core/DeltaTime.hpp"

UILayer::UILayer() { m_Canvas = nullptr; }

UILayer::~UILayer()
{
    if (m_Canvas) delete m_Canvas;
    NFSEngine::UIRenderer::Shutdown();
}

void UILayer::OnAttach() { Init(); }

void UILayer::OnDetach() { }

void UILayer::Init()
{
    float virtualWidth = 1920.0f;
    float virtualHeight = 1080.0f;

    NFSEngine::UIRenderer::Init();
    NFSEngine::UIRenderer::SetProjection(virtualWidth, virtualHeight);
    m_Canvas = new NFSEngine::Canvas();

    /* create fast bg
    NFSEngine::UI::ImageParameters baseBgParams;
    baseBgParams.position = glm::vec3(virtualWidth / 2.0f, virtualHeight / 2.0f, 0.0f);
    baseBgParams.width = virtualWidth;
    baseBgParams.height = virtualHeight;
    baseBgParams.color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, baseBgParams);

    NFSEngine::UI::ImageParameters shape1Params;
    shape1Params.position = glm::vec3(virtualWidth / 2.0f, virtualHeight / 2.0f, 0.1f);
    shape1Params.width = 1200;
    shape1Params.height = 800;
    shape1Params.color = glm::vec4(0.4f, 0.1f, 0.5f, 0.3f);
    m_BgShape1 = &NFSEngine::UI::Image(*m_Canvas, shape1Params);

    NFSEngine::UI::ImageParameters shape2Params;
    shape2Params.position = glm::vec3(virtualWidth / 2.0f, virtualHeight / 2.0f, 0.1f);
    shape2Params.width = 1000;
    shape2Params.height = 1000;
    shape2Params.color = glm::vec4(0.1f, 0.4f, 0.5f, 0.3f);
    m_BgShape2 = &NFSEngine::UI::Image(*m_Canvas, shape2Params);
    */

    NFSEngine::UI::LabelParameters labelParams;
    labelParams.position = glm::vec3(1500, 100, 2.0f);
    labelParams.text = "SCORE: 0";
    m_ScoreLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);

    NFSEngine::UI::ButtonParameters buttonParams;
    buttonParams.position = glm::vec3(1500, 200, 2.0f);
    buttonParams.width = 300;
    buttonParams.height = 70;
    buttonParams.text = "ADD SCORE";
    buttonParams.onClick = [this]()
    {
        m_Score += 100;
        if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>())
        {
            m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString = "SCORE: " + std::to_string(m_Score);
        }
    };
    NFSEngine::UI::Button(*m_Canvas, buttonParams);

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
}

void UILayer::OnUpdate(NFSEngine::DeltaTime deltaTime)
{
    m_AnimationTime += deltaTime.GetSeconds();
    if (m_AnimatedBar)
    {

        float animatedWidth = 200.0f + std::sin(m_AnimationTime * 3.0f) * 150.0f;
        m_AnimatedBar->Transform.Width = animatedWidth;
    }

    if (m_BgShape1 && m_BgShape2)
    {
        m_BgShape1->Transform.Position.x = 960.0f + std::sin(m_AnimationTime * 0.4f) * 400.0f;
        m_BgShape1->Transform.Position.y = 540.0f + std::cos(m_AnimationTime * 0.3f) * 200.0f;

        m_BgShape2->Transform.Position.x = 960.0f + std::cos(m_AnimationTime * 0.5f) * 300.0f;
        m_BgShape2->Transform.Position.y = 540.0f + std::sin(m_AnimationTime * 0.6f) * 300.0f;
    }

    Update();
}

void UILayer::OnRender() { Render(); }

void UILayer::Update() { m_Canvas->Update(); }

void UILayer::Render()
{
    NFSEngine::UIRenderer::Begin();
    m_Canvas->Draw();
    NFSEngine::UIRenderer::End();
}

void UILayer::OnEvent(NFSEngine::Event& e)
{

}