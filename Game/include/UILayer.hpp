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

    int LoseHeart() {
        if (m_Lives <= 0) return 0;
        m_Lives--;
        UpdateHeartVisuals();
        return m_Lives;
    }

    int GetLives() const { return m_Lives; }

private:
    void UpdateHeartVisuals();

    NFSEngine::Canvas* m_Canvas = nullptr;

    NFSEngine::UIObject* m_AnimatedBar = nullptr;
    NFSEngine::UIObject* m_ScoreLabel = nullptr;

    NFSEngine::UIObject* m_BgShape1 = nullptr;
    NFSEngine::UIObject* m_BgShape2 = nullptr;

    static constexpr int k_MaxLives = 3;
    NFSEngine::UIObject* m_Hearts[k_MaxLives] = {nullptr, nullptr, nullptr};
    int m_Lives = k_MaxLives;

    int m_Score = 0;
    float m_AnimationTime = 0.0f;
};
