#pragma once

#include "NFSEngine.h"
#include "UILayer.hpp"
#include <memory>
#include "Renderer/Skybox.hpp"
#include "Core/Audio/RhythmSequencer.hpp"
#include "Components/AudioPatternComponent.hpp"

class LayerExample : public NFSEngine::Layer {
public:
    LayerExample(UILayer* uiLayer);
    ~LayerExample() override;

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

    NFSEngine::GameObject* m_Player = nullptr;

    NFSEngine::GameObject* m_MovingCube = nullptr;
    NFSEngine::GameObject* m_MovingCube2 = nullptr;
    NFSEngine::GameObject* m_Floor = nullptr;
    NFSEngine::GameObject* m_Coin = nullptr;
    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    NFSEngine::DeltaTime m_DeltaTime;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;
    std::shared_ptr<NFSEngine::Shader> m_Shader;
    std::shared_ptr<NFSEngine::Skybox> m_Skybox;
    std::shared_ptr<NFSEngine::Shader> m_SkyboxShader;

    NFSEngine::RhythmSequencer m_Sequencer;
    NFSEngine::AudioPatternComponent* m_TestAudioComp = nullptr;

    UILayer* m_UILayer = nullptr;
};