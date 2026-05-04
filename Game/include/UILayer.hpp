#pragma once

#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "NFSEngine.h"
#include <memory>

class UILayer : public NFSEngine::Layer {
public:
    UILayer();
    virtual ~UILayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void Init();
    void Update();
    void Render();
    void OnEvent(NFSEngine::Event& e) override;

    void AddScore(int points) {
        m_Score += points;
        if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>()) {
            m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString =
                "SCORE: " + std::to_string(m_Score);
        }
    }

private:
    NFSEngine::Canvas* m_Canvas = nullptr;

    NFSEngine::UIObject* m_AnimatedBar = nullptr;
    NFSEngine::UIObject* m_ScoreLabel = nullptr;

    NFSEngine::UIObject* m_BgShape1 = nullptr;
    NFSEngine::UIObject* m_BgShape2 = nullptr;

    int m_Score = 0;
    float m_AnimationTime = 0.0f;
};