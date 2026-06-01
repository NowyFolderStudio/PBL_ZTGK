#pragma once

#include "NFSEngine.h"
#include <memory>
#include <vector>
#include "Renderer/Material.hpp"
#include "Renderer/Skybox.hpp"
#include "Core/Audio/RhythmSequencer.hpp"
#include "Components/AudioPatternComponent.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/EnvironmentMap.hpp"
#include "Components/HUDComponent.hpp"

// Forward declarations
class RhythmMover;
class RhythmPlatform;
class PianoKeyTrigger;

namespace NFSEngine {
    class Camera;
    class CameraController;
    class DirectionalLight;
    class SpotLight;
    class PointLight;
    class Material;
} // namespace NFSEngine

class LayerExample : public NFSEngine::Layer {
public:
    LayerExample();
    ~LayerExample() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    HUDComponent* m_HUD = nullptr;

    NFSEngine::GameObject* m_Player = nullptr;
    NFSEngine::GameObject* m_MovingCube = nullptr;
    NFSEngine::GameObject* m_MovingCube2 = nullptr;
    NFSEngine::GameObject* m_Floor = nullptr;
    NFSEngine::GameObject* m_PlayerModel = nullptr;

    std::shared_ptr<NFSEngine::Texture> m_RampTexture = nullptr;

    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    NFSEngine::DeltaTime m_DeltaTime;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;

    std::shared_ptr<NFSEngine::Shader> m_Shader;
    std::shared_ptr<NFSEngine::Shader> m_AudioShader;
    std::shared_ptr<NFSEngine::Shader> m_ToonShader;
    std::shared_ptr<NFSEngine::Shader> m_HazardShader;
    std::shared_ptr<NFSEngine::Skybox> m_Skybox;
    std::shared_ptr<NFSEngine::Shader> m_SkyboxShader;
    std::shared_ptr<NFSEngine::Shader> m_GoochShader;
    std::vector<std::shared_ptr<NFSEngine::Material>> m_AnimatedMaterials;

    std::unique_ptr<NFSEngine::EnvironmentMap> m_EnvironmentMap;

    NFSEngine::RhythmSequencer m_Sequencer;

    NFSEngine::Camera* m_CachedCamera = nullptr;
    NFSEngine::CameraController* m_CachedCameraController = nullptr;

    std::vector<RhythmPlatform*> m_CachedRhythmPlatforms;
    std::vector<RhythmMover*> m_CachedRhythmMovers;
    std::vector<PianoKeyTrigger*> m_CachedPianoKeys;

    float m_DeathPlaneY = -50.0f;
    glm::vec3 m_PlayerSpawnPosition = glm::vec3(0.0f, 2.0f, 0.0f);

    bool m_UseHDRI = false;
    std::shared_ptr<NFSEngine::Texture> m_HDRTexture;

    bool m_DrawDebug = false;

    std::shared_ptr<NFSEngine::Material> matAudio = nullptr;
    std::shared_ptr<NFSEngine::Material> matGoldPBR = nullptr;
    std::shared_ptr<NFSEngine::Material> matGramophone1 = nullptr;
    std::shared_ptr<NFSEngine::Material> matGramophone2 = nullptr;
    std::shared_ptr<NFSEngine::Material> matGramophone3 = nullptr;
    std::shared_ptr<NFSEngine::Material> matGramophone4 = nullptr;
    std::shared_ptr<NFSEngine::Material> matGramophone5 = nullptr;
};