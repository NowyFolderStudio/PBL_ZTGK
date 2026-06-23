#include "Layers/LayerGuga.hpp"

// Komponenty
#include "Components/AnimatorComponent.hpp"
#include "Components/CubeMesh.hpp"
#include "Components/Managers/ScoreManager.hpp"
#include "Components/Managers/LivesManager.hpp"
#include "Components/Aura/AuraManager.hpp"
#include "Components/CharacterController.hpp"
#include "Components/Controllers/AuraInputController.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/Camera.hpp"
#include "Components/CameraController.hpp"
#include "Components/DirectionalLight.hpp"
#include "Components/PointLight.hpp"
#include "Components/CasetteComponent.hpp"

// --- Specyficzne dla tej sceny ---
#include "Components/RotatingPlatform.hpp"
#include "Components/Enemy/BasicEnemy.hpp"
#include "Components/PlayerAttackComponent.hpp"
#include "Components/DestructibleComponent.hpp"
#include "Components/HUDComponent.hpp"
#include "Components/Managers/DialogueManager.hpp"
#include "Components/DialogueTriggerComponent.hpp"
#include "Components/MusicDirector.hpp"
#include "Components/MusicTriggerComponent.hpp"

// Core & Renderer
#include "Core/Log.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Renderer/Animation.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Material.hpp"
#include "Core/Application.hpp"

#include "GameManager.hpp"

#include <imgui.h>
#include <string>

using namespace NFSEngine;

LayerGuga::LayerGuga() {
    m_Player = nullptr;
    m_Scene = nullptr;
}

LayerGuga::~LayerGuga() { }

void LayerGuga::OnAttach() {
    m_Scene = std::make_unique<Scene>();
    m_HierarchyPanel = std::make_unique<SceneHierarchyPanel>(m_Scene.get());

    // --- Mened縠ry ---
    auto* livesManager = m_Scene->CreateGameObject("LivesManager");
    livesManager->SetTag(Tags::LivesManager);
    livesManager->AddComponent<LivesManager>();

    auto* scoreManager = m_Scene->CreateGameObject("ScoreManager");
    scoreManager->SetTag(Tags::ScoreManager);
    scoreManager->AddComponent<ScoreManager>();

    auto* auraManager = m_Scene->CreateGameObject("AuraManager");
    auraManager->AddComponent<AuraManager>();

    // --- HUD ---
    NFSEngine::GameObject* uiObj = m_Scene->CreateGameObject("HUD");
    m_HUD = &uiObj->AddComponent<HUDComponent>();

    // --- Shadery i Materia硑 ---
    m_Shader = Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");

    auto matSample = std::make_shared<Material>();
    matSample->AlbedoMap = Texture::Create("assets/textures/WoodFloor043/WoodFloor043_1K-PNG_Color.png");
    matSample->RoughnessMap = Texture::Create("assets/textures/WoodFloor043/WoodFloor043_1K-PNG_Roughness.png");
    matSample->MetallicMap = Texture::Create("assets/textures/WoodFloor043/WoodFloor043_1K-PNG_Metalness.png");
    matSample->AOMap = Texture::Create("assets/textures/WoodFloor043/WoodFloor043_1K-PNG_AmbientOcclusion.png");

    // --- 宺odowisko i Skybox ---
    std::vector<std::string> faces = { "assets/textures/skybox/testSkybox2/px.png", "assets/textures/skybox/testSkybox2/nx.png",
                                       "assets/textures/skybox/testSkybox2/py.png", "assets/textures/skybox/testSkybox2/ny.png",
                                       "assets/textures/skybox/testSkybox2/pz.png", "assets/textures/skybox/testSkybox2/nz.png" };
    m_Skybox = Skybox::Create(faces);
    m_SkyboxShader = Shader::Create("Skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    m_EnvironmentMap = std::make_unique<EnvironmentMap>();
    m_EnvironmentMap->LoadHDR("assets/textures/skybox/testSkybox2/skybox.hdr");
    m_EnvironmentMap->GenerateBRDFLUT();
    m_EnvironmentMap->GenerateIrradiance(m_Skybox->GetRendererID());
    m_EnvironmentMap->GeneratePrefilterMap(m_Skybox->GetRendererID());

    // --- O渨ietlenie ---
    auto* sunObj = m_Scene->CreateGameObject("Sun");
    auto& sunComp = sunObj->AddComponent<DirectionalLight>();
    sunComp.Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    sunComp.Color = glm::vec3(0.99f, 0.98f, 0.82f);
    sunComp.Intensity = 1.0f;

    // --- PODGA ---
    m_Floor = m_Scene->CreateGameObject("Floor_Zmoreq");
    m_Floor->GetTransform()->SetPosition({ 0.0f, -2.0f, 0.0f });
    m_Floor->GetTransform()->SetScale({ 50.0f, 1.0f, 50.0f });
    m_Floor->AddComponent<CubeMesh>(m_Shader, matSample);
    m_Floor->AddComponent<BoxCollider3DComponent>();

    auto enemyModel = std::make_shared<Model>("assets/models/glosnik/glosnik_wieza.fbx");

    // --- GRACZ ---
    auto animationShader = Shader::Create("AnimationShader", "assets/shaders/animation.vert", "assets/shaders/PBRShader.frag");
    auto capsuleModel = std::make_shared<Model>("assets/models/Player/Glowna_postac_baked_animations.fbx");

    m_Player = m_Scene->CreateGameObject("Player");
    m_Player->AddTag(Tags::Player);
    m_Player->GetTransform()->SetPosition(m_PlayerSpawnPosition);
    m_Player->AddComponent<CapsuleCollider3DComponent>();
    m_Player->AddComponent<RigidBody3DComponent>();

    m_Player->AddComponent<CharacterController>();
    m_Player->GetComponent<CharacterController>()->SpawnPosition = m_PlayerSpawnPosition;

    m_Player->AddComponent<AuraInputController>();

    auto* playerAoE = m_Scene->CreateGameObject("PlayerAttackZone");
    playerAoE->GetTransform()->SetParent(m_Player->GetTransform(), false);
    playerAoE->AddComponent<PlayerAttackComponent>();

    auto* playerModel = m_Scene->CreateGameObject("PlayerModel");
    playerModel->GetTransform()->SetParent(m_Player->GetTransform());
    playerModel->GetTransform()->SetPosition({ 0, -1.5f, 0 });
    playerModel->GetTransform()->SetScale({ 0.03, 0.03, 0.03 });

    auto playerMaterial = std::make_shared<Material>();
    playerMaterial->AlbedoMap = Texture::Create("assets/models/Player/Tekstura_postac_tshirt.png");

    auto& playerComp = playerModel->AddComponent<ModelComponent>(animationShader, playerMaterial);
    playerComp.AddLOD(capsuleModel, 10000.0f);
    playerModel->AddComponent<AnimatorComponent>();

    auto* rawModelData = playerComp.GetLODs()[0].ModelData.get();
    auto idleAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations.fbx", rawModelData, 1);
    auto runAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations.fbx", rawModelData, 2);

    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(idleAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(runAnimation);

    // audio
    NFSEngine::GameObject* directorObj = m_Scene->CreateGameObject("MusicDirector");
    auto& directorComp = directorObj->AddComponent<MusicDirector>();
    directorComp.InitMusic(m_Scene.get());

    // wrogowie

    m_EnemyGlitchShader
        = Shader::Create("EnemyGlitchShader", "assets/shaders/enemy_glitch.vert", "assets/shaders/enemy_glitch.frag");
    auto SpawnEnemy = [&](const std::string& name, glm::vec3 startPos, glm::vec3 endPos) {
        auto* enemy = m_Scene->CreateGameObject(name);
        enemy->GetTransform()->SetPosition(startPos);

        enemy->GetTransform()->SetScale({ 1.5f, 1.5f, 1.5f });

        auto& modelComp = enemy->AddComponent<ModelComponent>(m_EnemyGlitchShader, matSample);
        modelComp.AddLOD(enemyModel, 10000.0f);

        auto& boxCollider = enemy->AddComponent<BoxCollider3DComponent>();
        boxCollider.Offset = glm::vec3(0.0f, -0.75f, 0.0f);
        enemy->AddComponent<RigidBody3DComponent>();
        enemy->AddComponent<DestructibleComponent>(); // Otrzymuje obra縠nia
        enemy->AddTag(Tags::Enemy);

        auto& basicEnemyComp = enemy->AddComponent<BasicEnemy>(); // My渓i i atakuje
        basicEnemyComp.PatrolPointA = startPos;
        basicEnemyComp.PatrolPointB = endPos;
    };

    SpawnEnemy("Enemy_1", glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(15.0f, 0.0f, 5.0f));

    // Kasety test
    auto* casettePianoObj = m_Scene->CreateGameObject("Casette_Piano");
    casettePianoObj->GetTransform()->SetPosition(glm::vec3(10.0f, 1.0f, 5.0f));
    casettePianoObj->GetTransform()->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
    casettePianoObj->AddComponent<CubeMesh>(m_Shader, matSample);
    casettePianoObj->AddComponent<BoxCollider3DComponent>();

    auto& casettePianoComp = casettePianoObj->AddComponent<CasetteComponent>();
    casettePianoComp.TracksToUnlock.push_back("Piano");
    casettePianoComp.TracksToUnlock.push_back("SubPiano");

    auto* casetteBassObj = m_Scene->CreateGameObject("Casette_Bass");
    casetteBassObj->GetTransform()->SetPosition(glm::vec3(-10.0f, 1.0f, 5.0f));
    casetteBassObj->GetTransform()->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
    casetteBassObj->AddComponent<CubeMesh>(m_Shader, matSample);
    casetteBassObj->AddComponent<BoxCollider3DComponent>();

    auto& casetteBassComp = casetteBassObj->AddComponent<CasetteComponent>();

    casetteBassComp.TracksToUnlock.push_back("Bass");

    // zmiana trac體 test

    auto* trigger1 = m_Scene->CreateGameObject("Bass_Trigger_1");
    trigger1->GetTransform()->SetPosition(glm::vec3(50.0f, 1.0f, 0.0f));
    trigger1->AddComponent<BoxCollider3DComponent>();
    auto& triggerComp1 = trigger1->AddComponent<MusicTriggerComponent>();
    triggerComp1.TargetTrack = "Bass";

    // --- KAMERA ---
    auto* cameraObj = m_Scene->CreateGameObject("MainCamera");
    m_CachedCamera = &cameraObj->AddComponent<Camera>();
    m_CachedCameraController = &cameraObj->AddComponent<CameraController>();
    m_CachedCameraController->SetTarget(m_Player->GetTransform());

    m_Scene->MarkPhysicsDirty();

    uint32_t width = Application::Get().GetWindow().GetWidth();
    uint32_t height = Application::Get().GetWindow().GetHeight();
    Renderer::OnWindowResize(width, height);
}

void LayerGuga::OnDetach() { }

void LayerGuga::OnUpdate(DeltaTime deltaTime) {
    NFS_PROFILE_FUNCTION();
    m_DeltaTime = deltaTime;

    NFSEngine::DialogueManager::Get().Update(deltaTime.GetSeconds());

    // --- Debug Camera Logic ---
    bool editorActive = DebugCamera::IsActive();
    auto* playerController = m_Player ? m_Player->GetComponent<CharacterController>() : nullptr;
    static bool prevEditorActive = false;

    if (editorActive != prevEditorActive) {
        prevEditorActive = editorActive;
        if (editorActive) {
            if (m_CachedCameraController) m_CachedCameraController->SetActive(false);
            if (playerController) playerController->SetActive(false);
            if (m_CachedCamera) {
                auto* ct = m_CachedCamera->GetOwner()->GetTransform();
                DebugCamera::SyncFromTransform(ct->GetPosition(), ct->GetRotation());
            }
        } else {
            if (m_CachedCameraController) m_CachedCameraController->SetActive(true);
            if (playerController) playerController->SetActive(true);
        }
    }

    // --- Scene Update ---
    m_Scene->OnUpdate(deltaTime);

    // --- Player Logic ---
    if (m_Player) {
        float playerY = m_Player->GetTransform()->GetPosition().y;
        if (playerY < m_DeathPlaneY) {
            if (playerController) playerController->Respawn();
        }
    }

    if (editorActive && m_CachedCamera) {
        auto* camTransform = m_CachedCamera->GetOwner()->GetTransform();
        camTransform->SetPosition(DebugCamera::GetPosition());
        glm::vec3 euler = glm::degrees(glm::eulerAngles(DebugCamera::GetOrientation()));
        camTransform->SetRotation(euler);
    }

    static float timePassed = 0.0f;
    timePassed += deltaTime.GetSeconds();
}

void LayerGuga::OnRender() {
    if (m_CachedCamera) {
        Renderer::BeginScene(m_CachedCamera->GetViewMatrix(), m_CachedCamera->GetProjectionMatrix(),
                             m_CachedCamera->GetOwner()->GetTransform()->GetPosition(), m_Scene->GetDirLight(),
                             m_Scene->GetPointLights(), m_Scene->GetSpotLights(), m_EnvironmentMap.get());

        Renderer::DrawSkybox(m_Skybox, m_SkyboxShader);
        if (m_Scene) m_Scene->OnRender();

        Renderer::EndScene();
    }
    if (m_HUD) {
        m_HUD->RenderUI();
    }
}

void LayerGuga::OnImGuiRender() {
    if (!m_ShowImGui) {
        return;
    }

    ImGui::Begin("Diagnostic window");

    bool debugActive = NFSEngine::DebugCamera::IsActive();
    if (ImGui::Checkbox("Debug Camera", &debugActive)) {
        NFSEngine::DebugCamera::SetActive(debugActive);
    }
    if (debugActive) {
        ImGui::Text("WASD + RMB to fly, Q/E up/down");
        ImGui::Text("Scroll: zoom, Alt+LMB: orbit, MMB: pan");
    }
    ImGui::Separator();
    if (ImGui::Checkbox("Use HDR Texture?", &m_UseHDRI)) {
        if (m_UseHDRI) {
            m_EnvironmentMap->GenerateIrradiance(m_EnvironmentMap->GetEnvironmentMapID());
            m_EnvironmentMap->GeneratePrefilterMap(m_EnvironmentMap->GetEnvironmentMapID());
        } else {
            m_EnvironmentMap->GenerateIrradiance(m_Skybox->GetRendererID());
            m_EnvironmentMap->GeneratePrefilterMap(m_Skybox->GetRendererID());
        }
    }
    if (ImGui::Checkbox("Draw Debug Boxes?", &m_DrawDebug)) {
        Renderer::SetDrawDebug(m_DrawDebug);
    }
    ImGui::Separator();

    float averageFps = ImGui::GetIO().Framerate;
    float averageFrameTime = 1000.0f / averageFps;
    float currentFps = m_DeltaTime.GetFPS();
    float currentFrameTime = m_DeltaTime.GetMilliseconds();

    ImGui::Text("(Average) FPS: %.1f", averageFps);
    ImGui::Text("(Average) Frame Time: %.3f ms", averageFrameTime);
    ImGui::Text("(Current) FPS: %.1f", currentFps);
    ImGui::Text("(Current) Frame Time: %.3f ms", currentFrameTime);

    ImGui::Separator();
    ImGui::Text("GPU: %.3f ms", NFSEngine::Renderer::GetGPUTime());
    ImGui::Separator();

    bool cullingEnabled = NFSEngine::Renderer::IsFrustumCullingEnabled();
    if (ImGui::Checkbox("Frustum Culling", &cullingEnabled)) {
        NFSEngine::Renderer::SetFrustumCullingEnabled(cullingEnabled);
    }
    int cullingMode = NFSEngine::Renderer::GetFrustumCullingMode();
    const char* modes[] = { "Sphere", "AABB" };
    if (ImGui::Combo("Culling Mode", &cullingMode, modes, 2)) {
        NFSEngine::Renderer::SetFrustumCullingMode(cullingMode);
    }
    ImGui::Separator();

    auto stats = NFSEngine::Renderer::GetStats();
    ImGui::Text("Renderer Stats:");
    ImGui::Text("Draw Calls: %u", stats.drawCalls);
    ImGui::Text("Triangle count: %u", stats.triangleCount);
    ImGui::Text("State changes: %u", stats.stateChanges);

    static float values[90] = { 0 };
    static int values_offset = 0;
    values[values_offset] = currentFrameTime;
    values_offset = (values_offset + 1) % 90;
    ImGui::PlotLines("History of frameTime (ms)", values, 90, values_offset, nullptr, 0.0f, 33.3f, ImVec2(0, 80));

    ImGui::End();
    m_HierarchyPanel->OnImGuiRender();
}

void LayerGuga::OnEvent(Event& e) {
    if (m_CachedCameraController && !DebugCamera::IsActive()) {
        m_CachedCameraController->OnEvent(e);
    }

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& ev) {
        Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());
        return false;
    });

    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;
        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }
    }
}