#pragma once
#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include "UI/UIComponents.hpp"
#include <vector>
#include <memory>

enum class OutroState {
    FadeIn,
    Display,
    FadeOut,
    Finished
};

class OutroLayer : public NFSEngine::Layer {
public:
    OutroLayer();
    virtual ~OutroLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas = nullptr;
    NFSEngine::UIObject* m_ComicImage = nullptr;

    std::shared_ptr<NFSEngine::Texture> m_SkipTexture;
    NFSEngine::UIObject* m_SkipGraphic = nullptr;

    std::vector<std::shared_ptr<NFSEngine::Texture>> m_UnlockedComics;

    int m_CurrentComicIndex = 0;
    OutroState m_CurrentState = OutroState::FadeIn;
    float m_StateTimer = 0.0f;

    const float FADE_TIME = 1.5f;
    const float DISPLAY_TIME = 4.0f;
};