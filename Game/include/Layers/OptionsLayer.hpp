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
};
