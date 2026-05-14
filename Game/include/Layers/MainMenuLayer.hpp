#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include "UI/Canvas.hpp"

class MainMenuLayer : public NFSEngine::Layer {
public:
    MainMenuLayer();
    virtual ~MainMenuLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas = nullptr;
};