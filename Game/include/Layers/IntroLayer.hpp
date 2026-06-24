#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include "UI/PLMpegVideoDecoder.hpp"
#include <array>
#include <memory>

class IntroLayer : public NFSEngine::Layer {
public:
    IntroLayer();
    virtual ~IntroLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas = nullptr;

    std::unique_ptr<NFSEngine::PLMpegDecoder> m_VideoDecoder;
    std::shared_ptr<NFSEngine::Texture> m_VideoTexture;

    std::shared_ptr<NFSEngine::Texture> m_SkipTexture;
    NFSEngine::UIObject* m_SkipGraphic = nullptr;

    NFSEngine::UIObject* m_VideoImage = nullptr;

    float m_VideoAccumulator = 0.0f;
    bool m_videoEnded = false;
    bool m_Skipped = false;
    float m_SkipTimer = 0.0f;
};