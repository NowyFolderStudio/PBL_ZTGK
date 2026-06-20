#pragma once

#include "NFSEngine.h"
#include <memory>
#include <vector>
#include "Renderer/Skybox.hpp"
#include "Renderer/EnvironmentMap.hpp"
#include "Components/HUDComponent.hpp"

// Forward declarations
namespace NFSEngine {
    class Camera;
    class CameraController;
} // namespace NFSEngine

class LayerZmoreq : public NFSEngine::Layer {
public:
    LayerZmoreq();
    ~LayerZmoreq() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    std::unique_ptr<NFSEngine::Scene> m_Scene = nullptr;
    std::unique_ptr<NFSEngine::SceneHierarchyPanel> m_HierarchyPanel;
    NFSEngine::DeltaTime m_DeltaTime;

    NFSEngine::GameObject* m_Player = nullptr;
    NFSEngine::GameObject* m_Floor = nullptr;

    NFSEngine::GameObject* m_PusherWall = nullptr;

    std::shared_ptr<NFSEngine::Shader> m_Shader;
    std::shared_ptr<NFSEngine::Skybox> m_Skybox;
    std::shared_ptr<NFSEngine::Shader> m_SkyboxShader;
    std::shared_ptr<NFSEngine::Shader> m_EnemyGlitchShader;
    std::unique_ptr<NFSEngine::EnvironmentMap> m_EnvironmentMap;

    NFSEngine::Camera* m_CachedCamera = nullptr;
    NFSEngine::CameraController* m_CachedCameraController = nullptr;

    float m_DeathPlaneY = -50.0f;
    glm::vec3 m_PlayerSpawnPosition = glm::vec3(0.0f, 2.0f, 0.0f);

    bool m_UseHDRI = false;
    bool m_DrawDebug = false;

    HUDComponent* m_HUD = nullptr;
    bool m_ShowImGui = false;
};