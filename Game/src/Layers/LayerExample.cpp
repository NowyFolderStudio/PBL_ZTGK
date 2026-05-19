#include "Layers/LayerExample.hpp"

// Komponenty
#include "Components/CubeMesh.hpp"
#include "Components/CoinComponent.hpp"
#include "Components/HazardComponent.hpp"
#include "Components/ScoreManagerComponent.hpp"
#include "Components/LivesManagerComponent.hpp"
#include "Components/CharacterController.hpp"
#include "GameStateView.hpp"
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

// Core & Renderer
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Material.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"

#include "SceneLoader/SceneLoader.hpp"
#include "SceneLoader/CoinComponentLoader.hpp"
#include "GameManager.hpp"
#include "Core/Application.hpp"

#include <imgui.h>
#include <memory>

LayerExample::LayerExample(UILayer* uiLayer)
    : m_UILayer(uiLayer) {
    m_Player = nullptr;
    m_MovingCube = nullptr;
    m_Scene = nullptr;
}

LayerExample::~LayerExample() { NFSEngine::AudioEngine::Shutdown(); }

void LayerExample::OnAttach() {
    m_Scene = std::make_unique<NFSEngine::Scene>();
    m_HierarchyPanel = std::make_unique<NFSEngine::SceneHierarchyPanel>(m_Scene.get());

    NFSEngine::SceneLoader sceneLoader;

    sceneLoader.InitDefaultLoaders();

    sceneLoader.RegisterLoader(std::make_unique<CoinComponentLoader>());

    sceneLoader.LoadScene(m_Scene.get(), "assets/scenes/Level4_export.json");

    m_Shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
    m_AudioShader = NFSEngine::Shader::Create("AudioShader", "assets/shaders/audioShader.vert", "assets/shaders/PBRShader.frag");
    m_HazardShader
        = NFSEngine::Shader::Create("HazardShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
    m_HazardShader->Bind();
    m_HazardShader->SetVec4("u_ColorTint", glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));

    m_GoochShader
        = NFSEngine::Shader::Create("GoochShader", "assets/shaders/lightShader.vert", "assets/shaders/goochShader.frag");

    auto texCat = NFSEngine::Texture::Create("assets/textures/cat.png");
    auto matCat = std::make_shared<NFSEngine::Material>();
    matCat->AlbedoMap = texCat;

    auto texSample = NFSEngine::Texture::Create("assets/textures/sample.png");
    auto matSample = std::make_shared<NFSEngine::Material>();
    matSample->AlbedoMap = texSample;

    auto texWhite = NFSEngine::Texture::Create("assets/textures/white.png");
    auto matWhite = std::make_shared<NFSEngine::Material>();
    matWhite->AlbedoMap = texWhite;

    auto texBlack = NFSEngine::Texture::Create("assets/textures/black.png");
    auto matBlack = std::make_shared<NFSEngine::Material>();
    matBlack->AlbedoMap = texBlack;

    auto makePlatform = [&](const std::string& name, float x, float y, float z, float sizeX, float sizeZ,
                            float thickness = 1.0f) -> NFSEngine::GameObject* {
        NFSEngine::GameObject* obj = m_Scene->CreateGameObject(name);
        obj->GetTransform()->SetPosition({ x, y, z });
        obj->GetTransform()->SetScale({ sizeX, thickness, sizeZ });
        obj->AddComponent<NFSEngine::CubeMesh>(m_Shader, matSample);
        auto& col = obj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        col.Size = glm::vec3(sizeX, thickness, sizeZ);
        return obj;
    };

    auto makeCoin = [&](const std::string& name, float x, float y, float z) -> NFSEngine::GameObject* {
        NFSEngine::GameObject* obj = m_Scene->CreateGameObject(name);
        obj->GetTransform()->SetPosition({ x, y, z });
        obj->AddComponent<NFSEngine::CubeMesh>(m_Shader, matCat);
        obj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        obj->AddComponent<NFSEngine::CoinComponent>();
        return obj;
    };

    // Rotated Cube
    m_MovingCube = m_Scene->CreateGameObject("Rotated_Cube");
    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(m_Shader, matCat);
    m_MovingCube->GetTransform()->SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
    m_MovingCube->AddComponent<NFSEngine::BoxCollider3DComponent>();

    // Player
    auto capsuleModel = std::make_shared<NFSEngine::Model>("assets/models/Capsule/capsule.obj");

    m_Player = m_Scene->CreateGameObject("Player");
    m_Player->AddTag(NFSEngine::Tags::Player);
    m_Player->GetTransform()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    auto& playerComp = m_Player->AddComponent<NFSEngine::ModelComponent>(m_Shader, matCat);
    playerComp.AddLOD(capsuleModel, 10000.0f);
    m_Player->AddComponent<NFSEngine::CapsuleCollider3DComponent>();
    m_Player->AddComponent<NFSEngine::RigidBody3DComponent>();
    m_Player->AddComponent<CharacterController>();

    // PlayerModel
    NFSEngine::GLTextureParameters rampParams;
    rampParams.WrapS = GL_CLAMP_TO_EDGE;
    rampParams.WrapT = GL_CLAMP_TO_EDGE;
    rampParams.MinFilter = GL_NEAREST;
    rampParams.MagFilter = GL_NEAREST;
    rampParams.GenerateMipmaps = false;

    m_RampTexture = std::make_shared<NFSEngine::OpenGLTexture>("assets/textures/ramp/RampTexture.png", rampParams);
    auto playerModel = std::make_shared<NFSEngine::Model>("assets/models/Player/Player.obj");
    m_ToonShader = NFSEngine::Shader::Create("ToonShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
    m_PlayerModel = m_Scene->CreateGameObject("PlayerModel");
    m_PlayerModel->GetTransform()->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));
    auto texPlayer = NFSEngine::Texture::Create("assets/models/Player/playerModelTexture.JPEG");
    auto matPlayer = std::make_shared<NFSEngine::Material>();
    matPlayer->AlbedoMap = texPlayer;
    auto& toonComp = m_PlayerModel->AddComponent<NFSEngine::ModelComponent>(m_ToonShader, matPlayer);
    toonComp.AddLOD(playerModel, 10000.0f);

    // Sphere

    auto sphereModel = std::make_shared<NFSEngine::Model>("assets/models/ball/ball.obj");

    auto texSphereAlbedo = NFSEngine::Texture::Create("assets/models/ball/texture/Metal053B_1K-JPG_Color.jpg");
    auto texSphereNormal = NFSEngine::Texture::Create("assets/models/ball/texture/Metal053B_1K-JPG_NormalGL.jpg");
    auto texSphereMetallic = NFSEngine::Texture::Create("assets/models/ball/texture/Metal053B_1K-JPG_Metalness.jpg");
    auto texSphereRoughness = NFSEngine::Texture::Create("assets/models/ball/texture/Metal053B_1K-JPG_Roughness.jpg");
    auto texSphereAO = NFSEngine::Texture::Create("assets/models/ball/texture/Metal053B_1K-JPG_Displacement.jpg");
    /*
    auto texSphereAlbedo = NFSEngine::Texture::Create("assets/models/ball/texture/Metal048A_1K-JPG_Color.jpg");
    auto texSphereNormal = NFSEngine::Texture::Create("assets/models/ball/texture/Metal048A_1K-JPG_NormalGL.jpg");
    auto texSphereMetallic = NFSEngine::Texture::Create("assets/models/ball/texture/Metal048A_1K-JPG_Metalness.jpg");
    auto texSphereRoughness = NFSEngine::Texture::Create("assets/models/ball/texture/Metal048A_1K-JPG_Roughness.jpg");
    auto texSphereAO = NFSEngine::Texture::Create("assets/models/ball/texture/Metal048A_1K-JPG_Displacement.jpg");
    */

    auto matSpherePBR = std::make_shared<NFSEngine::Material>();
    matSpherePBR->AlbedoMap = texSphereAlbedo;
    matSpherePBR->NormalMap = texSphereNormal;
    matSpherePBR->MetallicMap = texSphereMetallic;
    matSpherePBR->RoughnessMap = texSphereRoughness;
    matSpherePBR->AOMap = texSphereAO;

    NFSEngine::GameObject* sphereObj = m_Scene->CreateGameObject("Center_PBR_Sphere");
    sphereObj->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    sphereObj->GetTransform()->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

    auto& sphereComp = sphereObj->AddComponent<NFSEngine::ModelComponent>(m_Shader, matSpherePBR);
    sphereComp.AddLOD(sphereModel, 10000.0f);

    // Static Cylinder
    auto cylinderModel = std::make_shared<NFSEngine::Model>("assets/models/Cylinder/cylinder.obj");

    NFSEngine::GameObject* cylinderObj = m_Scene->CreateGameObject("Static_Cylinder");
    cylinderObj->AddComponent<NFSEngine::CylinderCollider3DComponent>();
    auto& cylComp = cylinderObj->AddComponent<NFSEngine::ModelComponent>(m_AudioShader, matSample);
    cylComp.AddLOD(cylinderModel, 10000.0f);
    cylinderObj->GetTransform()->SetPosition({ 4.0f, 0.0f, 1.0f });

    // Gramophone
    auto gramophoneModel0 = std::make_shared<NFSEngine::Model>("assets/models/Gramophone/GramophoneHIGH.obj");
    auto gramophoneModel1 = std::make_shared<NFSEngine::Model>("assets/models/Gramophone/GramophoneMedium.obj");
    auto gramophoneModel2 = std::make_shared<NFSEngine::Model>("assets/models/Gramophone/GramophoneLOW.obj");

    NFSEngine::GameObject* gramophoneObj = m_Scene->CreateGameObject("Gramophone");
    auto& gramophoneComp = gramophoneObj->AddComponent<NFSEngine::ModelComponent>(m_Shader, matWhite);
    gramophoneComp.AddLOD(gramophoneModel0, 30.0f);
    gramophoneComp.AddLOD(gramophoneModel1, 45.0f);
    gramophoneComp.AddLOD(gramophoneModel2, 9999.9f);
    gramophoneObj->GetTransform()->SetPosition({ 23.0f, 2.0f, 0.0f });
    gramophoneObj->GetTransform()->SetScale({ 3.0f, 3.0f, 3.0f });
    gramophoneObj->GetTransform()->SetRotation({ 0.0f, 90.0f, 0.f });

    // Central Platform
    m_Floor = m_Scene->CreateGameObject("Floor");
    m_Floor->GetTransform()->SetPosition({ 0.0f, -2.0f, 0.0f });
    m_Floor->AddComponent<NFSEngine::CubeMesh>(m_Shader, matSample);
    m_Floor->AddComponent<NFSEngine::BoxCollider3DComponent>();
    m_Floor->GetTransform()->SetScale({ 20.0f, 1.0f, 20.0f });

    // Platform Ramp
    NFSEngine::GameObject* rampObj = m_Scene->CreateGameObject("Ramp");
    rampObj->GetTransform()->SetPosition({ -10.0f, -1.5f, 0.0f });
    rampObj->GetTransform()->SetRotation({ 0.0f, 0.0f, -30.0f });
    rampObj->GetTransform()->SetScale({ 12.0f, 1.0f, 4.0f });
    rampObj->AddComponent<NFSEngine::CubeMesh>(m_Shader, matSample);
    rampObj->AddComponent<NFSEngine::BoxCollider3DComponent>();

    // Lighting
    NFSEngine::GameObject* lightObj = m_Scene->CreateGameObject("PointLight_1");
    lightObj->GetTransform()->SetPosition({ 0.0f, 2.0f, 2.0f });
    auto& lightComp = lightObj->AddComponent<NFSEngine::PointLight>();
    lightComp.Color = { 1.0f, 0.3f, 0.3f };
    lightComp.Intensity = 120.0f;

    NFSEngine::GameObject* sunObj = m_Scene->CreateGameObject("Sun");
    auto& sunComp = sunObj->AddComponent<NFSEngine::DirectionalLight>();
    sunComp.Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    sunComp.Color = glm::vec3(0.2f, 0.3f, 0.92f);
    sunComp.Intensity = 0.0f;

    NFSEngine::GameObject* spotObj = m_Scene->CreateGameObject("MainSpotLight");
    spotObj->GetTransform()->SetPosition({ 0.0f, 3.5f, -3.0f });
    auto& spotComp = spotObj->AddComponent<NFSEngine::SpotLight>();
    spotComp.Color = { 0.1f, 0.2f, 0.93f };
    spotComp.Direction = { 0.0f, -1.0f, -0.5f };
    spotComp.Intensity = 130.0f;

    // Static Cube
    m_MovingCube2 = m_Scene->CreateGameObject("Static_Reference_Cube");
    m_MovingCube2->AddComponent<NFSEngine::CubeMesh>(m_Shader, matWhite);
    m_MovingCube2->GetTransform()->SetPosition({ -4.0f, -1.0f, 0.0f });
    m_MovingCube2->GetTransform()->SetScale({ 8.0f, 80.0f, 2.0f });
    m_MovingCube2->AddComponent<NFSEngine::BoxCollider3DComponent>();
    m_MovingCube2->AddTag(NFSEngine::Tags::WallJumpSurface);

    // Static Cube
    NFSEngine::GameObject* wall = m_Scene->CreateGameObject("wall");
    wall->AddComponent<NFSEngine::CubeMesh>(m_Shader, matWhite);
    wall->GetTransform()->SetPosition({ -4.0f, -1.0f, 10.0f });
    wall->GetTransform()->SetScale({ 8.0f, 80.0f, 2.0f });
    wall->AddComponent<NFSEngine::BoxCollider3DComponent>();
    wall->AddTag(NFSEngine::Tags::WallJumpSurface);

    // earth Object
    auto earthModel = std::make_shared<NFSEngine::Model>("assets/models/Earth/Sun.gltf");
    auto texEarth = NFSEngine::Texture::Create("assets/models/Earth/2k_earth_daymap.jpg");
    auto matEarth = std::make_shared<NFSEngine::Material>();
    matEarth->AlbedoMap = texEarth;

    NFSEngine::GameObject* earthObj = m_Scene->CreateGameObject("Earth");
    earthObj->GetTransform()->SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
    auto& earthComp = earthObj->AddComponent<NFSEngine::ModelComponent>(m_Shader, matEarth);
    earthComp.AddLOD(earthModel, 10000.0f);
    earthObj->AddComponent<NFSEngine::SphereCollider3DComponent>();

    auto& earthMover = earthObj->AddComponent<RhythmMover>();
    earthMover.TargetPattern = "PianoPattern1";
    earthMover.SetBasePosition(earthObj->GetTransform()->GetPosition());

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

    // Platforms

    const float platformY = -2.0f;
    const float stepSize = 4.0f;
    const float outerSize = 6.0f;
    const float coinY = -0.5f;

    // Top Platforms
    makePlatform("Platform_Step_Top", 0.0f, platformY, -15.0f, stepSize, stepSize);
    makePlatform("Platform_Top", 0.0f, platformY, -23.0f, outerSize, outerSize);

    // Bottom Platforms
    makePlatform("Platform_Step_Bot", 0.0f, platformY, 15.0f, stepSize, stepSize);
    makePlatform("Platform_Bot", 0.0f, platformY, 23.0f, outerSize, outerSize);

    // Left Platforms
    makePlatform("Platform_Step_Left", -15.0f, platformY, 0.0f, stepSize, stepSize);
    makePlatform("Platform_Left", -23.0f, platformY, 0.0f, outerSize, outerSize);

    // Right Platforms
    makePlatform("Platform_Step_Right", 15.0f, platformY, 0.0f, stepSize, stepSize);
    makePlatform("Platform_Right", 23.0f, platformY, 0.0f, outerSize, outerSize);

    // Coins
    makeCoin("Coin_Top", 0.0f, coinY, -23.0f);
    makeCoin("Coin_Bot", 0.0f, coinY, 23.0f);
    makeCoin("Coin_Left", -23.0f, coinY, 0.0f);
    makeCoin("Coin_Right", 23.0f, coinY, 0.0f);

    auto textureWhite2 = NFSEngine::Texture::Create("assets/textures/white.png");
    NFSEngine::GameObject* hazardCube = m_Scene->CreateGameObject("Hazard_Cube");
    hazardCube->GetTransform()->SetPosition(glm::vec3(5.0f, -1.0f, 5.0f));
    hazardCube->GetTransform()->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
    auto& hazardMesh = hazardCube->AddComponent<NFSEngine::CubeMesh>(m_HazardShader, matWhite);
    auto& hazardCol = hazardCube->AddComponent<NFSEngine::BoxCollider3DComponent>();
    auto& hazard = hazardCube->AddComponent<NFSEngine::HazardComponent>();

    // GameManager
    NFSEngine::GameObject* livesManager = m_Scene->CreateGameObject("LivesManager");
    livesManager->SetTag(NFSEngine::Tags::LivesManager);
    auto& livesComp = livesManager->AddComponent<NFSEngine::LivesManagerComponent>();

    NFSEngine::GameObject* scoreManager = m_Scene->CreateGameObject("ScoreManager");
    scoreManager->SetTag(NFSEngine::Tags::ScoreManager);
    auto& scoreComp = scoreManager->AddComponent<NFSEngine::ScoreManagerComponent>();

    auto gameState = std::make_shared<GameStateView>();
    gameState->data.score = scoreComp.GetScore();
    gameState->data.lives = livesComp.GetLives();
    scoreComp.m_OnScoreChanged = [gameState](int s) { gameState->data.score = s; };
    livesComp.m_OnLivesChanged = [gameState](int l) { gameState->data.lives = l; };
    if (m_UILayer) m_UILayer->SetGameState(gameState);

    // Audio
    NFSEngine::AudioEngine::Init();
    m_Sequencer.Start(120.0f);

    NFSEngine::GameObject* pianoObj = m_Scene->CreateGameObject("PianoTest");
    auto& audioComp = pianoObj->AddComponent<NFSEngine::AudioPatternComponent>();
    audioComp.LoadPattern("assets/audio/patterns/PianoPattern1.json", &m_Sequencer);
    m_TestAudioComp = &audioComp;
    audioComp.SetVolume(0.0);

    // PianoObject
    NFSEngine::GameObject* pianoManagerObj = m_Scene->CreateGameObject("PianoManager");
    auto& pianoLogic = pianoManagerObj->AddComponent<InteractivePiano>();
    pianoLogic.LoadPiano("assets/audio/sounds/piano01.ogg");

    NFSEngine::GameObject* pianoBase = m_Scene->CreateGameObject("PianoBase");
    pianoBase->AddComponent<NFSEngine::CubeMesh>(m_Shader, matBlack);
    pianoBase->GetTransform()->SetPosition(glm::vec3(44.f, -2.f, 0.f));
    pianoBase->GetTransform()->SetScale(glm::vec3(12.f, 1.f, 5.f));
    auto& pianoCol = pianoBase->AddComponent<NFSEngine::BoxCollider3DComponent>();

    for (int i = 0; i < 7; i++) {
        std::string keyName = "PianoKey" + std::to_string(i);
        NFSEngine::GameObject* keyObj = m_Scene->CreateGameObject(keyName);

        keyObj->AddComponent<NFSEngine::CubeMesh>(m_Shader, matWhite);
        keyObj->GetTransform()->SetPosition(glm::vec3(i * 1.7f + 39.f, -1.4f, 0.0f));
        keyObj->GetTransform()->SetScale(glm::vec3(1.6f, 0.2f, 4.0f));

        auto& col = keyObj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        col.IsTrigger = true;

        auto& keyTrigger = keyObj->AddComponent<PianoKeyTrigger>();
        keyTrigger.KeyIndex = i;
        keyTrigger.MainPiano = &pianoLogic;
        keyTrigger.TargetPlayer = m_Player;
        keyTrigger.SetBasePosition(keyObj->GetTransform()->GetPosition());
    }

    for (const auto& go : m_Scene->GetAllGameObjects()) {
        if (auto* cam = go->GetComponent<NFSEngine::Camera>()) m_CachedCamera = cam;
        if (auto* camCtrl = go->GetComponent<NFSEngine::CameraController>()) m_CachedCameraController = camCtrl;
        if (auto* dirLight = go->GetComponent<NFSEngine::DirectionalLight>()) m_CachedDirLight = dirLight;
        if (auto* spotLight = go->GetComponent<NFSEngine::SpotLight>()) m_CachedSpotLights.push_back(spotLight);
        if (auto* pointLight = go->GetComponent<NFSEngine::PointLight>()) m_CachedPointLights.push_back(pointLight);
        if (auto* mover = go->GetComponent<RhythmMover>()) m_CachedRhythmMovers.push_back(mover);
        if (auto* pianoKey = go->GetComponent<PianoKeyTrigger>()) m_CachedPianoKeys.push_back(pianoKey);
    }

    uint32_t width = NFSEngine::Application::Get().GetWindow().GetWidth();
    uint32_t height = NFSEngine::Application::Get().GetWindow().GetHeight();

    NFSEngine::Renderer::OnWindowResize(width, height);
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (GameManager::Get().GetCurrentState() == GameState::Paused) {
        return;
    }

    // DebugCamera
    bool editorActive = NFSEngine::DebugCamera::IsActive();
    auto* camController = m_CachedCameraController;
    auto* playerController = m_Player ? m_Player->GetComponent<CharacterController>() : nullptr;
    static bool prevEditorActive = false;

    if (editorActive != prevEditorActive) {
        prevEditorActive = editorActive;
        if (editorActive) {
            if (camController) camController->SetActive(false);
            if (playerController) playerController->SetActive(false);
            if (m_CachedCamera) {
                auto* ct = m_CachedCamera->GetOwner()->GetTransform();
                NFSEngine::DebugCamera::SyncFromTransform(ct->GetPosition(), ct->GetRotation());
            }
        } else {
            if (camController) camController->SetActive(true);
            if (playerController) playerController->SetActive(true);
        }
    }

    m_Sequencer.Update((float)deltaTime);
    m_DeltaTime = deltaTime;
    m_Scene->OnUpdate(deltaTime);

    if (editorActive && m_CachedCamera) {
        auto* camTransform = m_CachedCamera->GetOwner()->GetTransform();
        camTransform->SetPosition(NFSEngine::DebugCamera::GetPosition());
        glm::vec3 euler = glm::degrees(glm::eulerAngles(NFSEngine::DebugCamera::GetOrientation()));
        camTransform->SetRotation(euler);
    }

    if (m_TestAudioComp) {
        m_TestAudioComp->OnUpdate(deltaTime);
    }

    for (auto* mover : m_CachedRhythmMovers) {
        mover->OnUpdate(deltaTime);
    }

    for (auto* keyTrigger : m_CachedPianoKeys) {
        keyTrigger->OnUpdate(deltaTime);
    }
}

void LayerExample::OnRender() {
    if (m_CachedCamera) {
        auto bindLightsAndCamera = [&](std::shared_ptr<NFSEngine::Shader> currentShader) {
            currentShader->Bind();
            currentShader->SetVec3("viewPos", m_CachedCamera->GetOwner()->GetTransform()->GetPosition());

            if (m_EnvironmentMap) {
                m_EnvironmentMap->BindEnvironmentMaps(5, 6, 7);

                currentShader->SetInt("irradianceMap", 5);
                currentShader->SetInt("prefilterMap", 6);
                currentShader->SetInt("brdfLUT", 7);
            }

            if (m_CachedDirLight) {
                currentShader->SetVec3("dirLight.direction", m_CachedDirLight->Direction);
                currentShader->SetVec3("dirLight.color", m_CachedDirLight->Color);
                currentShader->SetFloat("dirLight.intensity", m_CachedDirLight->Intensity);
            }

            int lightIndex = 0;
            for (auto* light : m_CachedPointLights) {
                std::string base = "pointLights[" + std::to_string(lightIndex) + "].";
                currentShader->SetVec3(base + "position", light->GetTransform()->GetPosition());
                currentShader->SetVec3(base + "color", light->Color);
                currentShader->SetFloat(base + "intensity", light->Intensity);
                currentShader->SetFloat(base + "constant", light->Constant);
                currentShader->SetFloat(base + "linear", light->Linear);
                currentShader->SetFloat(base + "quadratic", light->Quadratic);

                lightIndex++;
                if (lightIndex >= 16) break;
            }
            currentShader->SetInt("activePointLights", lightIndex);

            int spotIndex = 0;
            for (auto* light : m_CachedSpotLights) {
                std::string base = "spotLights[" + std::to_string(spotIndex) + "].";
                currentShader->SetVec3(base + "position", light->GetTransform()->GetPosition());
                currentShader->SetVec3(base + "direction", light->Direction);
                currentShader->SetVec3(base + "color", light->Color);
                currentShader->SetFloat(base + "intensity", light->Intensity);
                currentShader->SetFloat(base + "cutOff", light->CutOff);
                currentShader->SetFloat(base + "outerCutOff", light->OuterCutOff);
                currentShader->SetFloat(base + "constant", light->Constant);
                currentShader->SetFloat(base + "linear", light->Linear);
                currentShader->SetFloat(base + "quadratic", light->Quadratic);

                spotIndex++;
                if (spotIndex >= 4) break;
            }
            currentShader->SetInt("activeSpotLights", spotIndex);
        };

        bindLightsAndCamera(m_Shader);
        bindLightsAndCamera(m_AudioShader);
        bindLightsAndCamera(m_ToonShader);

        bindLightsAndCamera(m_GoochShader);
        m_GoochShader->Bind();
        m_GoochShader->SetVec4("u_ColorTint", glm::vec4(1.0f));

        bindLightsAndCamera(m_HazardShader);

        m_Shader->Bind();
        m_Shader->SetVec4("u_ColorTint", glm::vec4(1.0f));

        m_AudioShader->Bind();
        m_AudioShader->SetVec4("u_ColorTint", glm::vec4(1.0f));
        float songPos = m_Sequencer.GetContinuousBeatTime();
        m_AudioShader->SetFloat("u_MusicTime", songPos);
        m_AudioShader->SetFloat("u_ScaleStrengthY", 0.3f);
        m_AudioShader->SetFloat("u_ScaleStrengthXZ", 0.0f);
        m_AudioShader->SetFloat("u_BendStrength", 0.0f);
        m_AudioShader->SetFloat("u_TwistStrength", 0.4f);

        m_HazardShader->Bind();
        m_HazardShader->SetVec4("u_ColorTint", glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));

        m_ToonShader->Bind();
        m_ToonShader->SetInt("texture1", 0);
        m_ToonShader->SetInt("rampTexture", 1);
        m_RampTexture->Bind(1);

        glm::vec3 camPos = m_CachedCamera->GetOwner()->GetTransform()->GetPosition();
        NFSEngine::Renderer::BeginScene(m_CachedCamera->GetViewMatrix(), m_CachedCamera->GetProjectionMatrix(), camPos);
        NFSEngine::Renderer::DrawSkybox(m_Skybox, m_SkyboxShader);
        if (m_Scene) m_Scene->OnRender();
        NFSEngine::Renderer::EndScene();
    }
}

void LayerExample::OnImGuiRender() {
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

    NFSEngine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<NFSEngine::WindowResizeEvent>([](NFSEngine::WindowResizeEvent& ev) {
        NFSEngine::Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());
        return false;
    });

    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;
        if (keyEvent.GetKeyCode() == NFSEngine::Key::P) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }
    }
}
