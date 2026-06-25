#pragma once

#include "NFSEngine.h"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Skybox.hpp"
#include "Renderer/EnvironmentMap.hpp"

// Komponenty
#include "Components/HUDComponent.hpp"
#include "Components/LoadingScreenComponent.hpp"
#include "Components/MusicDirector.hpp"
#include "SceneLoader/SceneLoader.hpp"

// Forward declarations dla optymalizacji
class RhythmMover;
class RhythmPlatform;
class PianoKeyTrigger;
class DancingWall;
class DartController;

namespace NFSEngine {
    class Camera;
    class CameraController;
    class DirectionalLight;
    class SpotLight;
    class PointLight;
    class Material;
} // namespace NFSEngine

class PlayableLayer : public NFSEngine::Layer {
public:
    // Konstruktor przyjmuje ścieżkę do pliku JSON
    PlayableLayer(const std::string& scenePath);
    virtual ~PlayableLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(NFSEngine::Event& e) override;

protected:
    std::string m_ScenePath;
    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    NFSEngine::DeltaTime m_DeltaTime;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;

    NFSEngine::GameObject* m_Player = nullptr;
    HUDComponent* m_HUD = nullptr;
    LoadingScreenComponent* m_LoadingScreen = nullptr;
    MusicDirector* m_CachedMusicDirector = nullptr;

    NFSEngine::Camera* m_CachedCamera = nullptr;
    NFSEngine::CameraController* m_CachedCameraController = nullptr;

    std::shared_ptr<NFSEngine::Shader> m_Shader;
    std::shared_ptr<NFSEngine::Shader> m_ToonShader;
    std::shared_ptr<NFSEngine::Skybox> m_Skybox;
    std::shared_ptr<NFSEngine::Shader> m_SkyboxShader;
    std::unique_ptr<NFSEngine::EnvironmentMap> m_EnvironmentMap;
    std::shared_ptr<NFSEngine::Texture> m_RampTexture;

    std::vector<RhythmPlatform*> m_CachedRhythmPlatforms;
    std::vector<RhythmMover*> m_CachedRhythmMovers;
    std::vector<PianoKeyTrigger*> m_CachedPianoKeys;
    std::vector<DancingWall*> m_CachedDancingWalls;
    std::vector<DartController*> m_CachedDartControllers;

    NFSEngine::SceneLoader m_SceneLoader;
    bool m_SceneReady = false;
    bool m_UseHDRI = false;

    float m_DeathPlaneY = -50.0f;
    glm::vec3 m_PlayerSpawnPosition = glm::vec3(-45.0f, 30.7f, 37.0f);

    virtual void FinalizeSceneSetup();
};