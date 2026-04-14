#pragma once

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
    NFSEngine::Cube* m_MyCube = nullptr;

    NFSEngine::GameObject* m_MovingCube = nullptr;
    NFSEngine::GameObject* m_MovingCube2 = nullptr;
    NFSEngine::GameObject* m_Floor = nullptr;
    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    NFSEngine::DeltaTime m_DeltaTime;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;
};