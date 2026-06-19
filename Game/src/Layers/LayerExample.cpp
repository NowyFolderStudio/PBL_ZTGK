#include "Layers/LayerExample.hpp"

// Komponenty
#include "Components/AnimatorComponent.hpp"
#include "Components/BounceComponent.hpp"
#include "Components/CubeMesh.hpp"
#include "Components/CoinComponent.hpp"
#include "Components/HazardComponent.hpp"
#include "Components/CheckpointComponent.hpp"
#include "Components/ParticleEmitterComponent.hpp"
#include "Components/ZoneCameraTriggerComponent.hpp"
#include "Components/Managers/ScoreManager.hpp"
#include "Components/Managers/LivesManager.hpp"
#include "Components/Aura/AuraManager.hpp"
#include "Components/CharacterController.hpp"
#include "Components/PlayerAttackComponent.hpp"
#include "Components/CharacterAnimationController.hpp"
#include "Components/Controllers/AuraInputController.hpp"
#include "Core/Log.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/RhythmMover.hpp"
#include "Components/InteractivePiano.hpp"
#include "Components/PianoKeyTrigger.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/Camera.hpp"
#include "Components/CameraController.hpp"
#include "Components/PointLight.hpp"
#include "Components/DirectionalLight.hpp"
#include "Components/SpotLight.hpp"
#include "Components/RhythmPlatform.hpp"
#include "Components/HUDComponent.hpp"
#include "Components/AuraPlatform.hpp"
#include "Components/PusherComponent.hpp"
#include "Components/DancingWall.hpp"
#include "Components/CasetteComponent.hpp"
#include "Components/ConsoleButtonComponent.hpp"
#include "Components/RotatingPlatform.hpp"

// Core & Renderer
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Core/AudioManager.hpp"
#include "Core/Scene.hpp"
#include "Renderer/Animation.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Material.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"

#include "SceneLoader/CasetteComponentLoader.hpp"
#include "SceneLoader/OutlineParametersLoader.hpp"
#include "SceneLoader/SceneLoader.hpp"
#include "SceneLoader/CoinComponentLoader.hpp"
#include "SceneLoader/CheckpointComponentLoader.hpp"
#include "SceneLoader/WallJumpLoader.hpp"
#include "SceneLoader/ZoneCameraTriggerComponentLoader.hpp"
#include "SceneLoader/DancingWallLoader.hpp"
#include "SceneLoader/RhythmPlatformLoader.hpp"
#include "SceneLoader/BounceComponentLoader.hpp"
#include "SceneLoader/AuraPlatformLoader.hpp"
#include "SceneLoader/ConsoleButtonLoader.hpp"
#include "GameManager.hpp"
#include "Core/Application.hpp"

#include <imgui.h>
#include <memory>
#include <vector>

LayerExample::LayerExample() {
    m_Player = nullptr;
    m_MovingCube = nullptr;
    m_Scene = nullptr;
}

LayerExample::~LayerExample() { }

void LayerExample::OnAttach() {
    m_Scene = std::make_unique<NFSEngine::Scene>();
    m_HierarchyPanel = std::make_unique<NFSEngine::SceneHierarchyPanel>(m_Scene.get());

    NFSEngine::GameObject* livesManager = m_Scene->CreateGameObject("LivesManager");
    livesManager->SetTag(NFSEngine::Tags::LivesManager);
    livesManager->AddComponent<LivesManager>();

    NFSEngine::GameObject* scoreManager = m_Scene->CreateGameObject("ScoreManager");
    scoreManager->SetTag(NFSEngine::Tags::ScoreManager);
    scoreManager->AddComponent<ScoreManager>();

    NFSEngine::GameObject* auraManager = m_Scene->CreateGameObject("AuraManager");
    auraManager->AddComponent<AuraManager>();

    NFSEngine::SceneLoader sceneLoader;

    m_SceneLoader.InitDefaultLoaders();
    m_SceneLoader.RegisterLoader(std::make_unique<CoinComponentLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<CheckpointComponentLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<ZoneCameraTriggerComponentLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<DancingWallLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<RhythmPlatformLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<WallJumpLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<BounceComponentLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<CasetteComponentLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<AuraPlatformLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<OutlineParameterstLoader>());
    m_SceneLoader.RegisterLoader(std::make_unique<ConsoleButtonLoader>());
    m_SceneLoader.LoadSceneAsync(m_Scene.get(), "assets/scenes/POziomix_v2_export.json");

    m_Shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");

    NFSEngine::GameObject* uiObj = m_Scene->CreateGameObject("HUD");
    m_HUD = &uiObj->AddComponent<HUDComponent>();

    NFSEngine::GameObject* loadingObj = m_Scene->CreateGameObject("LoadingScreen");
    m_LoadingScreen = &loadingObj->AddComponent<LoadingScreenComponent>();

    // ================= CREATING PLAYER START ==================
    auto capsuleModel = std::make_shared<NFSEngine::Model>("assets/models/Player/Glowna_postac_baked_animations.fbx");

    auto animationShader
        = NFSEngine::Shader::Create("AnimationShader", "assets/shaders/animation.vert", "assets/shaders/toonShader.frag");
    m_Player = m_Scene->CreateGameObject("Player");
    m_Player->AddTag(NFSEngine::Tags::Player);
    m_Player->GetTransform()->SetPosition(glm::vec3(-45.0f, 30.7f, 37.0f));
    // m_Player->GetTransform()->SetPosition(glm::vec3(115.0f, 100.0f, 5.0f));
    auto playerMaterial = std::make_shared<Material>();
    auto* playerModel = m_Scene->CreateGameObject("PlayerModel");
    playerModel->GetTransform()->SetParent(m_Player->GetTransform());
    playerModel->GetTransform()->SetPosition({ 0, -1.5f, 0 });
    playerModel->GetTransform()->SetScale({ 0.03, 0.03, 0.03 });
    playerMaterial->AlbedoMap = NFSEngine::Texture::Create("assets/models/Player/Tekstura_postac_tshirt_with_padding.jpg");
    playerMaterial->SetFloat("u_OutlineDepthThreshold", 0.5);
    playerMaterial->SetFloat("u_OutlineNormalThreshold", 0.35);
    auto& playerComp = playerModel->AddComponent<NFSEngine::ModelComponent>(animationShader, playerMaterial);
    playerComp.AddLOD(capsuleModel, 10000.0f);
    m_Player->AddComponent<NFSEngine::CapsuleCollider3DComponent>();
    m_Player->AddComponent<NFSEngine::RigidBody3DComponent>();
    m_Player->AddComponent<CharacterController>();
    m_Player->AddComponent<PlayerAttackComponent>();
    m_Player->GetComponent<CharacterController>()->SpawnPosition = m_PlayerSpawnPosition;
    playerModel->AddComponent<AnimatorComponent>();
    playerModel->AddComponent<CharacterAnimationController>();
    auto particleShader = Shader::Create("particleShader", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
    auto particleShader2 = Shader::Create("particleShader2", "assets/shaders/particle.vert", "assets/shaders/particle.frag");
    // Particle clouds
    auto particleCloudMaterial = std::make_shared<Material>();
    auto cloudParticleTexture = Texture::Create("assets/textures/particles/cloud.png");
    particleCloudMaterial->AlbedoMap = cloudParticleTexture;
    playerModel->AddComponent<ParticleEmitterComponent>(2000, particleShader, particleCloudMaterial);

    m_Player->AddComponent<AuraInputController>();

    auto* m = playerModel->GetComponent<ModelComponent>()->GetLODs()[0].ModelData.get();

    auto idleAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations(2).fbx", m, 5);
    auto runAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations(2).fbx", m, 10);
    auto jumpAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations(2).fbx", m, 9);
    auto fallAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations(2).fbx", m, 6);
    auto wallJumpAnimation = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations_wj.fbx", m, 4);
    auto dashAnimation
        = std::make_shared<Animation>("assets/models/Player/Glowna_postac_baked_animations_dash_skeleton.fbx", m, 5);

    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(idleAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(runAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(jumpAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(fallAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(wallJumpAnimation);
    playerModel->GetComponent<AnimatorComponent>()->AddAnimation(dashAnimation);

    NFSEngine::TextureParameters rampParams;
    rampParams.WrapS = NFSEngine::TextureWrap::Clamp;
    rampParams.WrapT = NFSEngine::TextureWrap::Clamp;
    rampParams.MinFilter = NFSEngine::TextureFilter::Nearest;
    rampParams.MagFilter = NFSEngine::TextureFilter::Nearest;
    rampParams.GenerateMipmaps = false;
    m_RampTexture = std::make_shared<NFSEngine::OpenGLTexture>("assets/textures/ramp/RampTexture.png", rampParams);
    playerMaterial->RampMap = m_RampTexture;
    m_ToonShader = NFSEngine::Shader::Create("ToonShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShader.frag");

    // ================= CREATING PLAYER END ==================

    // Lighting
    NFSEngine::GameObject* lightObj = m_Scene->CreateGameObject("PointLight_1");
    lightObj->GetTransform()->SetPosition({ -55.0f, 13.0f, 55.0f });
    auto& lightComp = lightObj->AddComponent<NFSEngine::PointLight>();
    lightComp.Color = { 1.0f, 0.3f, 0.3f };
    lightComp.Intensity = 120.0f;

    NFSEngine::GameObject* sunObj = m_Scene->CreateGameObject("Sun");
    auto& sunComp = sunObj->AddComponent<NFSEngine::DirectionalLight>();
    sunComp.Direction = glm::vec3(-0.2f, -1.0f, -0.6f);
    sunComp.Color = glm::vec3(0.99f, 0.98f, 0.82f);
    sunComp.Intensity = 1.0f;

    NFSEngine::GameObject* spotObj = m_Scene->CreateGameObject("MainSpotLight");
    spotObj->GetTransform()->SetPosition({ 0.0f, 3.5f, -3.0f });
    auto& spotComp = spotObj->AddComponent<NFSEngine::SpotLight>();
    spotComp.Color = { 0.1f, 0.2f, 0.93f };
    spotComp.Direction = { 0.0f, -1.0f, -0.5f };
    spotComp.Intensity = 130.0f;

    // Camera
    NFSEngine::GameObject* cameraObj = m_Scene->CreateGameObject("MainCamera");
    cameraObj->AddComponent<NFSEngine::Camera>();
    auto& controller = cameraObj->AddComponent<NFSEngine::CameraController>();
    controller.SetTarget(m_Player->GetTransform());

    // Skybox
    std::vector<std::string> faces = { "assets/textures/skybox/testSkybox2/px.png", "assets/textures/skybox/testSkybox2/nx.png",
                                       "assets/textures/skybox/testSkybox2/py.png", "assets/textures/skybox/testSkybox2/ny.png",
                                       "assets/textures/skybox/testSkybox2/pz.png", "assets/textures/skybox/testSkybox2/nz.png" };
    m_Skybox = NFSEngine::Skybox::Create(faces);
    m_SkyboxShader = NFSEngine::Shader::Create("Skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    m_EnvironmentMap = std::make_unique<NFSEngine::EnvironmentMap>();

    m_EnvironmentMap->LoadHDR("assets/textures/skybox/testSkybox2/skybox.hdr");

    m_EnvironmentMap->GenerateBRDFLUT();

    if (m_UseHDRI) {
        m_EnvironmentMap->GenerateIrradiance(m_EnvironmentMap->GetEnvironmentMapID());
        m_EnvironmentMap->GeneratePrefilterMap(m_EnvironmentMap->GetEnvironmentMapID());
    } else {
        m_EnvironmentMap->GenerateIrradiance(m_Skybox->GetRendererID());
        m_EnvironmentMap->GeneratePrefilterMap(m_Skybox->GetRendererID());
    }

    NFSEngine::GameObject* checkpoint = m_Scene->CreateGameObject("Checkpoint_Start");
    checkpoint->GetTransform()->SetPosition(glm::vec3(0.0f, 7.0f, 40.0f));
    checkpoint->GetTransform()->SetScale(glm::vec3(20.0f, 15.0f, 0.1f));
    checkpoint->AddComponent<NFSEngine::BoxCollider3DComponent>();
    checkpoint->AddComponent<CheckpointComponent>();

    NFSEngine::GameObject* camZone = m_Scene->CreateGameObject("CamZone_Piano");
    camZone->GetTransform()->SetPosition(glm::vec3(44.0f, 1.0f, 0.0f));
    camZone->GetTransform()->SetScale(glm::vec3(14.0f, 5.0f, 5.0f));
    camZone->AddComponent<NFSEngine::BoxCollider3DComponent>();
    auto& camTrigger = camZone->AddComponent<ZoneCameraTriggerComponent>();
    camTrigger.CustomYaw = 90.0f;
    camTrigger.CustomPitch = 45.0f;
    camTrigger.CustomDistance = 15.0f;

    // Audio
    m_Sequencer.Start(120.0f);

    NFSEngine::GameObject* bassObj1 = m_Scene->CreateGameObject("BassMusicPlayer1");
    auto& bassComp1 = bassObj1->AddComponent<NFSEngine::AudioPatternComponent>();
    bassComp1.TrackName = "Bass";
    bassComp1.LoadPattern("assets/audio/patterns/BassPatternPrototype.json", &m_Sequencer);
    bassComp1.SetVolume(1.0);
    NFSEngine::AudioManager::RegisterPattern(&bassComp1);

    NFSEngine::GameObject* bassObj2 = m_Scene->CreateGameObject("BassMusicPlayer2");
    auto& bassComp2 = bassObj2->AddComponent<NFSEngine::AudioPatternComponent>();
    bassComp2.TrackName = "Bass";
    bassComp2.LoadPattern("assets/audio/patterns/BassPatternPrototype2.json", &m_Sequencer);
    bassComp2.SetVolume(1.0);
    NFSEngine::AudioManager::RegisterPattern(&bassComp2);

    NFSEngine::GameObject* pianoObj = m_Scene->CreateGameObject("PianoMusicPlayer1");
    auto& audioComp = pianoObj->AddComponent<NFSEngine::AudioPatternComponent>();
    audioComp.TrackName = "Piano";
    audioComp.LoadPattern("assets/audio/patterns/PianoPattern1.json", &m_Sequencer);
    audioComp.SetVolume(0.5);
    NFSEngine::AudioManager::RegisterPattern(&audioComp);

    NFSEngine::GameObject* pianoObj2 = m_Scene->CreateGameObject("PianoMusicPlayer2");
    auto& audioComp2 = pianoObj2->AddComponent<NFSEngine::AudioPatternComponent>();
    audioComp2.TrackName = "Piano";
    audioComp2.LoadPattern("assets/audio/patterns/PianoPattern2.json", &m_Sequencer);
    audioComp2.SetVolume(0.5);
    NFSEngine::AudioManager::RegisterPattern(&audioComp2);

    NFSEngine::AudioManager::SetActivePatternInTrack("Bass", "BassPatternPrototype");
    NFSEngine::AudioManager::SetActivePatternInTrack("Piano", "PianoPattern1");

    // PianoObject
    NFSEngine::GameObject* pianoManagerObj = m_Scene->CreateGameObject("PianoManager");
    auto& pianoLogic = pianoManagerObj->AddComponent<InteractivePiano>();
    pianoLogic.LoadPiano("assets/audio/sounds/piano01.ogg");

    // auto cdShader = NFSEngine::Shader::Create("CDShader", "assets/shaders/lightShader.vert", "assets/shaders/CDShader.frag");

    // auto matCD = std::make_shared<NFSEngine::Material>();
    // matCD->name = "DiffractionMaterial";
    // matCD->AlbedoColor = glm::vec3(0.1f, 0.1f, 0.1f);
    // matCD->Metallic = 1.0f;
    // matCD->Roughness = 0.15f;

    // matCD->SetInt("u_UseDiffraction", 1);
    // matCD->SetFloat("u_DiffractionDistance", 2000.0f);
    // matCD->SetFloat("u_DiffractionStrength", 2.5f);

    // auto* spinningCD = m_Scene->CreateGameObject("SpinningCD");
    // spinningCD->GetTransform()->SetScale(glm::vec3(9.0f, 9.0f, 9.0f));
    // spinningCD->GetTransform()->SetPosition(glm::vec3 { -35.0f, 20.0f, 50.0f });
    // spinningCD->AddComponent<CylinderCollider3DComponent>();

    // auto cdModel = std::make_shared<NFSEngine::Model>("assets/models/CDZTGK.fbx");
    // auto& cdModelComp = spinningCD->AddComponent<NFSEngine::ModelComponent>(cdShader, matCD);
    // cdModelComp.AddLOD(cdModel, 100000.0f);

    // auto& rotPlatformComp = spinningCD->AddComponent<RotatingPlatform>();
    // rotPlatformComp.RotationSpeed = glm::vec3(0.0f, 90.0f, 0.0f);

    uint32_t width = NFSEngine::Application::Get().GetWindow().GetWidth();
    uint32_t height = NFSEngine::Application::Get().GetWindow().GetHeight();

    NFSEngine::Renderer::OnWindowResize(width, height);
}

void LayerExample::FinalizeSceneSetup() {
    int gameObjectCounter = m_Scene->GetAllGameObjects().size();
    for (int i = 0; i < gameObjectCounter; i++) {
        GameObject* go = m_Scene->GetAllGameObjects()[i].get();
        ModelComponent* component = go->GetComponent<ModelComponent>();
        if (component) {
            auto mat = component->GetMaterial(0);
            if (mat && mat->name == "AnimationMaterial") {
                m_AnimatedMaterials.push_back(mat);
            }
        }

        if (auto* cam = go->GetComponent<NFSEngine::Camera>()) m_CachedCamera = cam;
        if (auto* camCtrl = go->GetComponent<NFSEngine::CameraController>()) m_CachedCameraController = camCtrl;
        if (auto* mover = go->GetComponent<RhythmMover>()) m_CachedRhythmMovers.push_back(mover);
        if (auto* pianoKey = go->GetComponent<PianoKeyTrigger>()) m_CachedPianoKeys.push_back(pianoKey);

        if (auto* platform = go->GetComponent<RhythmPlatform>()) {
            m_CachedRhythmPlatforms.push_back(platform);
            platform->OnAwake();
        }

        if (auto* wall = go->GetComponent<DancingWall>()) {
            m_CachedDancingWalls.push_back(wall);
            wall->OnAwake();
        }
    }

    m_Scene->MarkPhysicsDirty();
}

void LayerExample::OnDetach() { m_Scene.reset(); }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    NFS_PROFILE_FUNCTION();

    if (m_SceneLoader.IsLoading()) {
        m_SceneLoader.ProcessLoading();

        if (m_LoadingScreen) {
            m_LoadingScreen->UpdateProgress(m_SceneLoader.GetProgress());
        }

        return;
    }
    if (!m_SceneReady) {
        if (m_LoadingScreen) {
            m_LoadingScreen->SetVisible(false);
        }

        FinalizeSceneSetup();
        m_SceneReady = true;
    }

    bool isPaused = (GameManager::Get().GetCurrentState() == GameState::Paused);
    static bool prevWasPaused = false;

    if (isPaused) {
        prevWasPaused = true;
        return;
    }

    if (prevWasPaused && m_CachedCameraController) {
        m_CachedCameraController->ResetMouseDelta();
    }
    prevWasPaused = false;

    // DebugCamera
    bool editorActive = NFSEngine::DebugCamera::IsActive();
    auto* camController = m_CachedCameraController;
    auto* playerController = m_Player ? m_Player->GetComponent<CharacterController>() : nullptr;
    static bool prevEditorActive = false;

    if (editorActive != prevEditorActive) {
        prevEditorActive = editorActive;
        if (editorActive) {
            if (camController) camController->SetActive(false);
            if (playerController) playerController->SetActive(true);
            if (m_CachedCamera) {
                auto* ct = m_CachedCamera->GetOwner()->GetTransform();
                NFSEngine::DebugCamera::SyncFromTransform(ct->GetPosition(), ct->GetRotation());
            }
        } else {
            if (camController) camController->SetActive(true);
            if (playerController) playerController->SetActive(true);
        }
    }

    {
        NFS_PROFILE_SCOPE("LayerExample: Audio Sequencer Update");
        m_Sequencer.Update((float)deltaTime);

        NFSEngine::AudioManager::Update(deltaTime);
    }
    m_DeltaTime = deltaTime;

    {
        NFS_PROFILE_SCOPE("LayerExample: Scene Update");
        m_Scene->OnUpdate(deltaTime);
    }
    {
        NFS_PROFILE_SCOPE("LayerExample: Player & Logic Update");
        if (m_Player) {
            auto* lm = m_Scene->FindWithTag(NFSEngine::Tags::LivesManager);
            auto* livesComp = lm ? lm->GetComponent<LivesManager>() : nullptr;
            auto* cc = m_Player->GetComponent<CharacterController>();
            float playerY = m_Player->GetTransform()->GetPosition().y;

            if (playerY < m_DeathPlaneY || (livesComp && livesComp->GetLives() <= 0)) {
                if (cc) cc->Respawn();
                if (livesComp) livesComp->ResetLives();
            }
        }

        if (editorActive && m_CachedCamera) {
            auto* camTransform = m_CachedCamera->GetOwner()->GetTransform();
            camTransform->SetPosition(NFSEngine::DebugCamera::GetPosition());
            glm::vec3 euler = glm::degrees(glm::eulerAngles(NFSEngine::DebugCamera::GetOrientation()));
            camTransform->SetRotation(euler);
        }

        for (auto* mover : m_CachedRhythmMovers) {
            mover->OnUpdate(deltaTime);
        }

        for (auto* keyTrigger : m_CachedPianoKeys) {
            keyTrigger->OnUpdate(deltaTime);
        }
    }

    {
        NFS_PROFILE_SCOPE("LayerExample: Material Uniforms Update");
        float songPos = m_Sequencer.GetContinuousBeatTime();
        // matAudio->SetFloat("u_MusicTime", songPos);
        // matGramophone1->SetFloat("u_MusicTime", songPos);
        // matGramophone2->SetFloat("u_MusicTime", songPos);
        // matGramophone3->SetFloat("u_MusicTime", songPos);
        // matGramophone4->SetFloat("u_MusicTime", songPos);
        // matGramophone5->SetFloat("u_MusicTime", songPos);
        for (auto mat : m_AnimatedMaterials) {
            mat->SetFloat("u_MusicTime", songPos);
        }
    }

    for (auto* wall : m_CachedDancingWalls) {
        wall->OnUpdate(deltaTime);
    }
}

void LayerExample::OnRender() {
    if (m_SceneLoader.IsLoading()) {
        NFSEngine::Renderer::GetAPI().SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        NFSEngine::Renderer::GetAPI().Clear();

        if (m_LoadingScreen) {
            m_LoadingScreen->RenderUI();
        }

        return;
    }

    if (m_CachedCamera) {
        NFSEngine::Renderer::BeginScene(m_CachedCamera->GetViewMatrix(), m_CachedCamera->GetProjectionMatrix(),
                                        m_CachedCamera->GetOwner()->GetTransform()->GetPosition(), m_Scene->GetDirLight(),
                                        m_Scene->GetPointLights(), m_Scene->GetSpotLights(), m_EnvironmentMap.get());

        NFSEngine::Renderer::DrawSkybox(m_Skybox, m_SkyboxShader);
        if (m_Scene) m_Scene->OnRender();
        NFSEngine::Renderer::EndScene();
    }

    if (m_HUD) {
        m_HUD->RenderUI();
    }
}

void LayerExample::OnImGuiRender() {
    if (!m_ShowImGui) return;

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
    bool sortingEnabled = NFSEngine::Renderer::s_SortingEnabled;
    if (ImGui::Checkbox("Sorting Enabled", &sortingEnabled)) {
        NFSEngine::Renderer::s_SortingEnabled = sortingEnabled;
    }
    bool lodEnabled = NFSEngine::Renderer::s_LodEnabled;
    if (ImGui::Checkbox("LOD Enabled", &lodEnabled)) {
        NFSEngine::Renderer::s_LodEnabled = lodEnabled;
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

void LayerExample::OnEvent(NFSEngine::Event& e) {
    if (GameManager::Get().GetCurrentState() == GameState::Paused) {
        return;
    }

    if (m_CachedCameraController && !NFSEngine::DebugCamera::IsActive()) {
        m_CachedCameraController->OnEvent(e);
    }

    for (auto* mover : m_CachedRhythmMovers) {
        mover->OnEvent(e);
    }

    for (auto* platform : m_CachedRhythmPlatforms) {
        platform->OnEvent(e);
    }

    for (auto* wall : m_CachedDancingWalls) {
        wall->OnEvent(e);
    }

    NFSEngine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<NFSEngine::WindowResizeEvent>([](NFSEngine::WindowResizeEvent& ev) {
        NFSEngine::Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());
        return false;
    });

    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;

        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }

        if (keyEvent.GetKeyCode() == NFSEngine::Key::F1) {
            m_ShowImGui = !m_ShowImGui;
            e.Handled = true;
        }
    }
}