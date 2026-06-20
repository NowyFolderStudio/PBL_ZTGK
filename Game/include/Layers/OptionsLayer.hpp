#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>

#include "UI/Canvas.hpp"

class OptionsLayer : public NFSEngine::Layer {
public:
    OptionsLayer();
    virtual ~OptionsLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas;
    int m_CurrentResIndex = 2;
    std::vector<std::pair<int, int>> m_Resolutions = { { 1280, 720 }, { 1600, 900 }, { 1920, 1080 } };

    int m_FocusedIndex = 0;
    int m_FocusedRow = 0;

    void SyncResolutionIndex();
    void BuildUI();
};
