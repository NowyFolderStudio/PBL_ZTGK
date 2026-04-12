#pragma once

#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "NFSEngine.h"
#include <memory>

class LayerExample : public NFSEngine::Layer
{
public:
    LayerExample();
    virtual ~LayerExample() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

    void Init();
    void Update();
    void Render();
    void OnEvent(NFSEngine::Event& e) override;

private:
    std::shared_ptr<NFSEngine::Shader> m_MyShader = nullptr;
    std::shared_ptr<NFSEngine::Texture> m_MyTexture;
    NFSEngine::Cube* m_MyCube = nullptr;

    NFSEngine::GameObject* m_MovingCube = nullptr;
    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
};