#pragma once

#include <NFSEngine.h>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <algorithm>

#include "Components/Component.hpp"
#include "Core/Text.hpp"
#include "UI/UIComponents.hpp"
#include "UI/UIFactory.hpp"
#include "UI/PLMpegVideoDecoder.hpp"

class LoadingScreenComponent : public NFSEngine::Component {
public:
    LoadingScreenComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) {
        m_Canvas = new NFSEngine::Canvas();
        InitUI();
    }

    ~LoadingScreenComponent() override {
        if (m_Canvas) delete m_Canvas;
    }

    std::string GetName() const override { return "LoadingScreenComponent"; }

    void RenderUI() {
        if (m_Canvas && m_IsVisible) {
            NFSEngine::UIRenderer::Begin();
            m_Canvas->Draw();
            NFSEngine::UIRenderer::End();
        }
    }

    void SetVisible(bool visible) { m_IsVisible = visible; }

    void UpdateProgress(float progress) {
        if (m_ProgressBarFill) {
            float clampedProgress = std::clamp(progress, 0.0f, 1.0f);
            m_ProgressBarFill->Transform.Width = m_MaxBarWidth * clampedProgress;
        }
    }

protected:
    void OnAwake() override {
        if (m_Canvas == nullptr) {
            m_Canvas = new NFSEngine::Canvas();
        }
        if (!m_UIInitialized) {
            InitUI();
        }
    }

    void OnStart() override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_IsVisible) return;

        if (m_Canvas) {
            m_Canvas->Update();
        }

        if (m_VideoDecoder && m_VideoTexture) {
            m_VideoAccumulator += deltaTime.GetSeconds();
            float frameTime = 1.0f / static_cast<float>(m_VideoDecoder->GetFPS());

            if (m_VideoAccumulator >= frameTime) {
                if (m_VideoDecoder->ReadNextFrame()) {
                    uint32_t dataSize = m_VideoDecoder->GetWidth() * m_VideoDecoder->GetHeight() * 3;
                    m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);
                }
                m_VideoAccumulator -= frameTime;
            }
        }
        m_TextAnimTimer += deltaTime.GetSeconds();
        if (m_TextAnimTimer >= 0.5f) {
            m_TextAnimTimer -= 0.5f;

            m_DotCount++;
            if (m_DotCount > 3) {
                m_DotCount = 1;
            }

            if (m_ProgressText && m_ProgressText->HasComponent<NFSEngine::TextComponent>()) {
                auto* textComp = m_ProgressText->GetComponent<NFSEngine::TextComponent>();

                std::string dots(m_DotCount, '.');
                textComp->TextString = "Loading" + dots;
            }
        }
    }

private:
    void InitUI() {
        float screenWidth = 1920.0f;
        float screenHeight = 1080.0f;

        m_MaxBarWidth = screenWidth;

        float barHeight = 15.0f;

        float centerX = screenWidth / 2.0f;

        float barY = screenHeight - (barHeight / 2.0f);

        m_VideoDecoder = std::make_unique<NFSEngine::PLMpegDecoder>();
        if (m_VideoDecoder->OpenFile("assets/videos/loading.mpg")) {
            NFSEngine::TextureParameters texParams;
            texParams.Channels = 3;
            texParams.GenerateMipmaps = false;
            texParams.WrapS = NFSEngine::TextureWrap::Clamp;
            texParams.WrapT = NFSEngine::TextureWrap::Clamp;
            texParams.sRGB = false;

            m_VideoTexture = NFSEngine::Texture::Create(m_VideoDecoder->GetWidth(), m_VideoDecoder->GetHeight(), texParams);
            if (m_VideoTexture && m_VideoDecoder->ReadNextFrame()) {
                uint32_t dataSize = m_VideoDecoder->GetWidth() * m_VideoDecoder->GetHeight() * 3;
                m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);
            }
        } else {
            NFS_ERROR("[LoadingScreenComponent]: Can't load video!");
        }

        NFSEngine::UI::ImageParameters bgParams;
        bgParams.position = glm::vec3(centerX, screenHeight / 2.0f, 0.1f);
        bgParams.width = screenWidth;
        bgParams.height = -screenHeight;
        bgParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        if (m_VideoTexture) {
            bgParams.texture = m_VideoTexture;
        } else {
            bgParams.color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
        }

        m_VideoBackground = &NFSEngine::UI::Image(*m_Canvas, bgParams);
        float leftEdgeX = 0.0f;

        NFSEngine::UI::ImageParameters barFillParams;
        barFillParams.position = glm::vec3(leftEdgeX, barY, 0.3f);
        barFillParams.width = 0.0f;
        barFillParams.height = barHeight;
        barFillParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_ProgressBarFill = &NFSEngine::UI::Image(*m_Canvas, barFillParams);

        m_ProgressBarFill->Transform.Pivot.x = 0.0f;

        NFSEngine::UI::LabelParameters textParams;
        float textX = screenWidth - 225.0f;

        textParams.position = glm::vec3(textX, barY - 30.0f, 0.4f);
        textParams.font = new NFSEngine::Text("assets/fonts/Super-Pandora.ttf");
        textParams.text = "Loading...";
        textParams.scale = 1.0f;
        textParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_ProgressText = &NFSEngine::UI::Label(*m_Canvas, textParams);
        m_ProgressText->Transform.Pivot = glm::vec2(0, 0);

        m_UIInitialized = true;
    }

    bool m_UIInitialized = false;
    NFSEngine::Canvas* m_Canvas = nullptr;
    bool m_IsVisible = true;
    float m_MaxBarWidth = 800.0f;

    NFSEngine::UIObject* m_ProgressBarFill = nullptr;
    NFSEngine::UIObject* m_ProgressText = nullptr;

    std::unique_ptr<NFSEngine::PLMpegDecoder> m_VideoDecoder;
    std::shared_ptr<NFSEngine::Texture> m_VideoTexture;
    float m_VideoAccumulator = 0.0f;
    NFSEngine::UIObject* m_VideoBackground = nullptr;

    float m_TextAnimTimer = 0.0f;
    int m_DotCount = 3;
};