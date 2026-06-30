#include "Layers/OutroLayer.hpp"
#include "Core/Log.hpp"
#include "GameManager.hpp"
#include "UI/UIRenderer.hpp"

OutroLayer::OutroLayer() { m_Canvas = nullptr; }

OutroLayer::~OutroLayer() {
    if (m_Canvas) delete m_Canvas;
}

void OutroLayer::OnAttach() {
    NFSEngine::UIRenderer::Init();
    m_Canvas = new NFSEngine::Canvas();

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position
        = glm::vec3(NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f, -0.1f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH;
    bgParams.height = NFSEngine::UIRenderer::VIRTUAL_HEIGHT;
    bgParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    NFSEngine::UI::Image(*m_Canvas, bgParams);

    NFSEngine::TextureParameters texParams;
    texParams.WrapS = NFSEngine::TextureWrap::Clamp;
    texParams.WrapT = NFSEngine::TextureWrap::Clamp;
    texParams.sRGB = false;

    int finalScore = GameManager::Get().PlayerScore;
    int maxPossibleScore = 132;

    if (maxPossibleScore <= 0) maxPossibleScore = 1;

    float completionPercentage = (static_cast<float>(finalScore) / maxPossibleScore) * 100.0f;

    if (completionPercentage >= 30.0f) {
        m_UnlockedComics.push_back(NFSEngine::Texture::Create("assets/textures/ui/outro/comic1.png", texParams));
    }

    if (completionPercentage >= 60.0f) {
        m_UnlockedComics.push_back(NFSEngine::Texture::Create("assets/textures/ui/outro/comic2.png", texParams));
    }

    if (completionPercentage >= 99.9f) {
        m_UnlockedComics.push_back(NFSEngine::Texture::Create("assets/textures/ui/outro/comic3.png", texParams));
    }

    float targetHeight = NFSEngine::UIRenderer::VIRTUAL_HEIGHT * 0.95f;
    float aspectRatio = 2480.0f / 3508.0f;
    float targetWidth = targetHeight * aspectRatio;

    NFSEngine::UI::ImageParameters imgParams;

    imgParams.position
        = glm::vec3(NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f, 0.0f);
    imgParams.width = targetWidth;
    imgParams.height = targetHeight;

    if (!m_UnlockedComics.empty()) {
        imgParams.texture = m_UnlockedComics[0];
    }

    imgParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

    m_ComicImage = &NFSEngine::UI::Image(*m_Canvas, imgParams);
    m_ComicImage->Transform.Pivot = glm::vec2(0.5f, 0.5f);

    NFSEngine::TextureParameters skipTexParams;
    skipTexParams.WrapS = NFSEngine::TextureWrap::Clamp;
    skipTexParams.WrapT = NFSEngine::TextureWrap::Clamp;
    skipTexParams.sRGB = false;

    m_SkipTexture = NFSEngine::Texture::Create("assets/textures/ui/tutorial_continue.png", skipTexParams);

    NFSEngine::UI::ImageParameters skipParams;
    skipParams.position
        = glm::vec3(NFSEngine::UIRenderer::VIRTUAL_WIDTH - 250.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 100.0f, 0.5f);
    skipParams.width = 381.5f;
    skipParams.height = 540.0f;
    skipParams.texture = m_SkipTexture;
    skipParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

    m_SkipGraphic = &NFSEngine::UI::Image(*m_Canvas, skipParams);

    m_CurrentComicIndex = 0;
    m_StateTimer = 0.0f;
    m_CurrentState = OutroState::FadeIn;
}

void OutroLayer::OnDetach() { }

void OutroLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Canvas) m_Canvas->Update();

    if (m_UnlockedComics.empty() || m_CurrentState == OutroState::Finished) {
        GameManager::Get().RequestStateChange(GameState::MainMenu);
        return;
    }

    m_StateTimer += deltaTime.GetSeconds();

    if (m_ComicImage && m_ComicImage->HasComponent<NFSEngine::ImageComponent>()) {
        auto* imgComp = m_ComicImage->GetComponent<NFSEngine::ImageComponent>();

        NFSEngine::ImageComponent* skipComp = nullptr;
        if (m_SkipGraphic && m_SkipGraphic->HasComponent<NFSEngine::ImageComponent>()) {
            skipComp = m_SkipGraphic->GetComponent<NFSEngine::ImageComponent>();
        }

        switch (m_CurrentState) {
        case OutroState::FadeIn: {
            float alpha = m_StateTimer / 1.5f;
            if (alpha >= 1.0f) {
                alpha = 1.0f;
                m_CurrentState = OutroState::Display;
                m_StateTimer = 0.0f;
            }
            imgComp->Color.a = alpha;
            if (skipComp) skipComp->Color.a = alpha;
            break;
        }
        case OutroState::Display: {
            bool advancePressed = false;

            if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::Space)) {
                advancePressed = true;
            }

            if (NFSEngine::Input::IsControllerButtonPressed(0, NFSEngine::ControllerButtons::A)) {
                advancePressed = true;
            }

            if (advancePressed && m_StateTimer > 0.2f) {
                m_CurrentState = OutroState::FadeOut;
                m_StateTimer = 0.0f;
            }
            break;
        }
        case OutroState::FadeOut: {
            float alpha = 1.0f - (m_StateTimer / 1.5f);
            if (alpha <= 0.0f) {
                alpha = 0.0f;
                m_CurrentComicIndex++;

                if (m_CurrentComicIndex < m_UnlockedComics.size()) {
                    imgComp->TexturePtr = m_UnlockedComics[m_CurrentComicIndex];
                    m_CurrentState = OutroState::FadeIn;
                } else {
                    m_CurrentState = OutroState::Finished;
                }
                m_StateTimer = 0.0f;
            }
            imgComp->Color.a = alpha;
            if (skipComp) skipComp->Color.a = alpha;
            break;
        }
        default:
            break;
        }
    }
}

void OutroLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();

    if (m_Canvas) {
        m_Canvas->Draw();
    }

    NFSEngine::UIRenderer::End();
}

void OutroLayer::OnEvent(NFSEngine::Event& e) { }