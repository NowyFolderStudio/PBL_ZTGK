#pragma once

#include "NFSEngine.h"
#include "UILayer.hpp"
#include <memory>
#include <vector>
#include "Renderer/Skybox.hpp"
#include "Core/Audio/RhythmSequencer.hpp"
#include "Components/AudioPatternComponent.hpp"

// Forward declarations
class RhythmMover;
class PianoKeyTrigger;

namespace NFSEngine {
    class Camera;
    class CameraController;
    class DirectionalLight;
    class SpotLight;
    class PointLight;
} // namespace NFSEngine

class LayerExample : public NFSEngine::Layer {
public:
    LayerExample(UILayer* uiLayer);
    ~LayerExample() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::GameObject* m_Player = nullptr;
    NFSEngine::GameObject* m_MovingCube = nullptr;
    NFSEngine::GameObject* m_MovingCube2 = nullptr;
    NFSEngine::GameObject* m_Floor = nullptr;
    NFSEngine::GameObject* m_Coin = nullptr;
    NFSEngine::GameObject* m_PlayerModel = nullptr;

    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    NFSEngine::DeltaTime m_DeltaTime;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;

    std::shared_ptr<NFSEngine::Shader> m_Shader;
    std::shared_ptr<NFSEngine::Shader> m_AudioShader;
    std::shared_ptr<NFSEngine::Skybox> m_Skybox;
    std::shared_ptr<NFSEngine::Shader> m_SkyboxShader;

    NFSEngine::RhythmSequencer m_Sequencer;
    NFSEngine::AudioPatternComponent* m_TestAudioComp = nullptr;

    // Cache wskaźników
    NFSEngine::Camera* m_CachedCamera = nullptr;
    NFSEngine::CameraController* m_CachedCameraController = nullptr;
    NFSEngine::DirectionalLight* m_CachedDirLight = nullptr;
    NFSEngine::SpotLight* m_CachedSpotLight = nullptr;

    std::vector<NFSEngine::PointLight*> m_CachedPointLights;
    std::vector<RhythmMover*> m_CachedRhythmMovers;
    std::vector<PianoKeyTrigger*> m_CachedPianoKeys;

    UILayer* m_UILayer = nullptr;
};