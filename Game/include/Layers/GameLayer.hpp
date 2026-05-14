#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>

class GameLayer : public NFSEngine::Layer {
public:
    GameLayer();
    virtual ~GameLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void OnEvent(NFSEngine::Event& e) override;

private:
};
