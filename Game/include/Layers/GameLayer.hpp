#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>

#include "UILayer.hpp"
#include "Components/Camera.hpp"

class GameLayer : public NFSEngine::Layer {
public:
    GameLayer(const std::string& levelPath, UILayer* uiLayer);
    virtual ~GameLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void OnEvent(NFSEngine::Event& e) override;

private:
    std::string m_LevelFilePath;
    UILayer* m_UILayer;
    std::unique_ptr<NFSEngine::Scene> m_Scene;

    NFSEngine::Camera* m_CachedCamera = nullptr;
};
