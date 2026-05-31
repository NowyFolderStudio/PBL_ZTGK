#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>

struct GameStateView;

class HUDLayer : public NFSEngine::Layer {
public:
    HUDLayer();
    virtual ~HUDLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void OnEvent(NFSEngine::Event& e) override;

    void SetGameState(std::shared_ptr<GameStateView> view);

private:
    void SyncFromGameState();
    void AnimateElements(NFSEngine::DeltaTime deltaTime);
    void UpdateHeartVisuals();

    void InitScoreUI();
    void InitHeartsUI();
    void InitAuraUI();

    NFSEngine::Canvas* m_Canvas = nullptr;

    NFSEngine::UIObject* m_AnimatedBar = nullptr;
    NFSEngine::UIObject* m_ScoreLabel = nullptr;

    NFSEngine::UIObject* m_BgShape1 = nullptr;
    NFSEngine::UIObject* m_BgShape2 = nullptr;

    std::vector<NFSEngine::UIObject*> m_Hearts;

    std::shared_ptr<GameStateView> m_GameState;

    float m_AnimationTime = 0.0f;
};
