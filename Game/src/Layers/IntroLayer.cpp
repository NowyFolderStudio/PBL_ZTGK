#include "Layers/IntroLayer.hpp"
#include "Core/Log.hpp"
#include "GameManager.hpp"
#include <glm/ext/vector_float2.hpp>
#include <memory>

IntroLayer::IntroLayer() { m_Canvas = nullptr; }

IntroLayer::~IntroLayer() {
    if (m_Canvas) delete m_Canvas;
}

void IntroLayer::OnAttach() {
    NFSEngine::UIRenderer::Init();
    m_Canvas = new NFSEngine::Canvas();
    m_VideoDecoder = std::make_unique<NFSEngine::PLMpegDecoder>();
    if (m_VideoDecoder->OpenFile("assets/videos/intro.mpg", false)) {
        NFSEngine::TextureParameters texParams;
        texParams.Channels = 3;
        texParams.GenerateMipmaps = false;
        texParams.WrapS = NFSEngine::TextureWrap::Clamp;
        texParams.WrapT = NFSEngine::TextureWrap::Clamp;
        texParams.sRGB = false;

        m_VideoTexture = NFSEngine::Texture::Create(m_VideoDecoder->GetWidth(), m_VideoDecoder->GetHeight(), texParams);

        if (m_VideoTexture && m_VideoDecoder->ReadNextFrame()) {
            uint32_t dataSize = m_VideoDecoder->GetDataSize();
            m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);
        }

        NFS_INFO("We got into intro gz!!");
    } else {
        NFS_CORE_ERROR("[MainMenuLayer]: Can't load intro video!");
    }

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position
        = glm::vec3(NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f, 0.0f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH;
    bgParams.height = -NFSEngine::UIRenderer::VIRTUAL_HEIGHT;
    bgParams.texture = m_VideoTexture;
    bgParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_VideoImage = &NFSEngine::UI::Image(*m_Canvas, bgParams);
}

void IntroLayer::OnDetach() { }

void IntroLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Canvas) m_Canvas->Update();

    if (m_VideoDecoder && m_VideoTexture) {
        m_VideoAccumulator += deltaTime.GetSeconds();
        float frameTime = 1.0f / static_cast<float>(m_VideoDecoder->GetFPS());

        int framesDecodedThisTick = 0;

        while (m_VideoAccumulator >= frameTime && framesDecodedThisTick < 2) {
            if (m_VideoDecoder->ReadNextFrame()) {
                uint32_t dataSize = m_VideoDecoder->GetDataSize();
                m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);

                m_VideoAccumulator -= frameTime;
                framesDecodedThisTick++;
            } else {
                break;
            }
        }

        if (m_VideoDecoder->IsFinished()) {
            GameManager::Get().RequestStateChange(GameState::Playing);
            NFS_INFO("Intro ended");
            return;
        }

        if (m_VideoAccumulator > frameTime * 2.0f) {
            m_VideoAccumulator = 0.0f;
        }
    }
}

void IntroLayer::OnRender() {

    NFSEngine::UIRenderer::Begin();

    if (m_Canvas) {
        m_Canvas->Draw();
    }

    NFSEngine::UIRenderer::End();
}

void IntroLayer::OnEvent(NFSEngine::Event& e) { }
