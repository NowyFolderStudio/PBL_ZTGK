#pragma once

#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Components/ScoreManager.hpp"
#include "Components/LivesManager.hpp"
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

    ScoreManager& GetScoreManager() { return m_ScoreManager; }
    LivesManager& GetLivesManager() { return m_LivesManager; }

private:
    void UpdateHeartVisuals();

    NFSEngine::Canvas* m_Canvas = nullptr;

    NFSEngine::UIObject* m_AnimatedBar = nullptr;
    NFSEngine::UIObject* m_ScoreLabel = nullptr;

    NFSEngine::UIObject* m_BgShape1 = nullptr;
    NFSEngine::UIObject* m_BgShape2 = nullptr;

    NFSEngine::UIObject* m_Hearts[LivesManager::k_MaxLives] = {nullptr, nullptr, nullptr};

    ScoreManager m_ScoreManager;
    LivesManager m_LivesManager;

    float m_AnimationTime = 0.0f;
};
