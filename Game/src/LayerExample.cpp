#include "LayerExample.hpp"

// Komponenty
#include "Components/CubeMesh.hpp"
#include "Components/CoinComponent.hpp"
#include "Components/HazardComponent.hpp"
#include "Components/CharacterController.hpp"
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
#include "Platforms/OpenGL/OpenGLTexture.hpp"

#include <imgui.h>

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

    m_Shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/lightShader.frag");
    m_AudioShader
        = NFSEngine::Shader::Create("AudioShader", "assets/shaders/audioShader.vert", "assets/shaders/lightShader.frag");
    m_HazardShader
        = NFSEngine::Shader::Create("HazardShader", "assets/shaders/lightShader.vert", "assets/shaders/lightShader.frag");
    m_HazardShader->Bind();
    m_HazardShader->SetVec4("u_ColorTint", glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));

    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");
    auto texture2 = NFSEngine::Texture::Create("assets/textures/sample.png");
    auto textureWhite = NFSEngine::Texture::Create("assets/textures/white.png");
    auto textureBlack = NFSEngine::Texture::Create("assets/textures/black.png");

    auto makePlatform = [&](const std::string& name, float x, float y, float z, float sizeX, float sizeZ,
                            float thickness = 1.0f) -> NFSEngine::GameObject* {
        NFSEngine::GameObject* obj = m_Scene->CreateGameObject(name);
        obj->GetTransform()->SetPosition({ x, y, z });
        obj->GetTransform()->SetScale({ sizeX, thickness, sizeZ });
        obj->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture2);
        auto& col = obj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        col.Size = glm::vec3(sizeX, thickness, sizeZ);
        return obj;
    };

    auto makeCoin = [&](const std::string& name, float x, float y, float z) -> NFSEngine::GameObject* {
        NFSEngine::GameObject* obj = m_Scene->CreateGameObject(name);
        obj->GetTransform()->SetPosition({ x, y, z });
        obj->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture);
        auto& col = obj->AddComponent<NFSEngine::BoxCollider3DComponent>();
        col.IsTrigger = true;
        auto& coin = obj->AddComponent<NFSEngine::CoinComponent>();
        coin.SetTarget(m_Player);
        coin.OnCollected = [this]() {
            if (m_UILayer) m_UILayer->AddScore(1000);
        };
        return obj;
    };

    // Rotated Cube
    m_MovingCube = m_Scene->CreateGameObject("Rotated_Cube");
    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture);
    m_MovingCube->GetTransform()->SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
    m_MovingCube->AddComponent<NFSEngine::BoxCollider3DComponent>();

    // Player
    auto capsuleModel = std::make_shared<NFSEngine::Model>("assets/models/Capsule/capsule.obj");

    m_Player = m_Scene->CreateGameObject("Player");
    m_Player->GetTransform()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
    m_Player->AddComponent<NFSEngine::ModelComponent>(capsuleModel, m_Shader, texture);
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

    auto rampTexture = std::make_shared<NFSEngine::OpenGLTexture>("textures/ramp/.png", rampParams);
    auto playerModel = std::make_shared<NFSEngine::Model>("assets/models/Player/Player.obj");
    m_ToonShader = NFSEngine::Shader::Create("ToonShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShader.frag");
    m_PlayerModel = m_Scene->CreateGameObject("PlayerModel");
    m_PlayerModel->GetTransform()->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));
    auto playerModelTexture = NFSEngine::Texture::Create("assets/models/Player/playerModelTexture.JPEG");
    m_PlayerModel->AddComponent<NFSEngine::ModelComponent>(playerModel, m_ToonShader, playerModelTexture);

    // Static Cylinder
    auto cylinderModel = std::make_shared<NFSEngine::Model>("assets/models/Cylinder/cylinder.obj");

    NFSEngine::GameObject* cylinderObj = m_Scene->CreateGameObject("Static_Cylinder");
    cylinderObj->AddComponent<NFSEngine::CylinderCollider3DComponent>();
    cylinderObj->AddComponent<NFSEngine::ModelComponent>(cylinderModel, m_AudioShader, texture2);
    cylinderObj->GetTransform()->SetPosition({ 4.0f, 0.0f, 1.0f });

    // Central Platform
    m_Floor = m_Scene->CreateGameObject("Floor");
    m_Floor->GetTransform()->SetPosition({ 0.0f, -2.0f, 0.0f });
    m_Floor->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture2);
    m_Floor->AddComponent<NFSEngine::BoxCollider3DComponent>();
    m_Floor->GetComponent<NFSEngine::BoxCollider3DComponent>()->Size = glm::vec3(20.0f, 1.0f, 20.0f);
    m_Floor->GetTransform()->SetScale({ 20.0f, 1.0f, 20.0f });

    // Lighting
    NFSEngine::GameObject* lightObj = m_Scene->CreateGameObject("PointLight_1");
    lightObj->GetTransform()->SetPosition({ 0.0f, 2.0f, 2.0f });
    auto& lightComp = lightObj->AddComponent<NFSEngine::PointLight>();
    lightComp.Color = { 1.0f, 0.8f, 0.5f };
    lightComp.Intensity = 3.0f;

    NFSEngine::GameObject* sunObj = m_Scene->CreateGameObject("Sun");
    auto& sunComp = sunObj->AddComponent<NFSEngine::DirectionalLight>();
    sunComp.Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    sunComp.Color = glm::vec3(1.0f, 0.9f, 0.8f);
    sunComp.Intensity = 0.8f;

    NFSEngine::GameObject* spotObj = m_Scene->CreateGameObject("MainSpotLight");
    spotObj->GetTransform()->SetPosition({ 0.0f, 1.5f, 0.0f });
    auto& spotComp = spotObj->AddComponent<NFSEngine::SpotLight>();
    spotComp.Color = { 1.0f, 1.0f, 1.0f };
    spotComp.Direction = { 0.0f, -1.0f, -0.5f };
    spotComp.Intensity = 5.0f;

    // Static Cube
    m_MovingCube2 = m_Scene->CreateGameObject("Static_Reference_Cube");
    m_MovingCube2->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture);
    m_MovingCube2->GetTransform()->SetPosition({ -4.0f, -1.0f, 0.0f });
    m_MovingCube2->AddComponent<NFSEngine::BoxCollider3DComponent>();

    auto earthModel = std::make_shared<NFSEngine::Model>("assets/models/Earth/Sun.gltf");
    auto earthTexture = NFSEngine::Texture::Create("assets/models/Earth/2k_earth_daymap.jpg");

    NFSEngine::GameObject* earthObj = m_Scene->CreateGameObject("Earth");
    earthObj->GetTransform()->SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
    earthObj->AddComponent<NFSEngine::ModelComponent>(earthModel, m_Shader, earthTexture);
    earthObj->AddComponent<NFSEngine::SphereCollider3DComponent>();

    auto& earthMover = earthObj->AddComponent<RhythmMover>();
    earthMover.TargetPattern = "PianoPattern1";
    earthMover.SetBasePosition(earthObj->GetTransform()->GetPosition());

    // Camera
    NFSEngine::GameObject* cameraObj = m_Scene->CreateGameObject("MainCamera");
    cameraObj->AddComponent<NFSEngine::Camera>();
    auto& controller = cameraObj->AddComponent<NFSEngine::CameraController>();
    controller.SetTarget(m_Player->GetTransform());

    if (auto* characterController = m_Player->GetComponent<CharacterController>()) {
        characterController->SetCameraTransform(cameraObj->GetTransform());
    }

    // Skybox
    std::vector<std::string> faces = { "assets/textures/skybox/testSkybox/px.png", "assets/textures/skybox/testSkybox/nx.png",
                                       "assets/textures/skybox/testSkybox/py.png", "assets/textures/skybox/testSkybox/ny.png",
                                       "assets/textures/skybox/testSkybox/pz.png", "assets/textures/skybox/testSkybox/nz.png" };
    m_Skybox = NFSEngine::Skybox::Create(faces);
    m_SkyboxShader = NFSEngine::Shader::Create("Skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    const auto& gameObjects = m_Scene->GetAllGameObjects();
    for (const auto& go : gameObjects) {
        if (go.get() != m_Player) {
            if (auto* control = go->GetComponent<CharacterController>()) {
                control->SetActive(false);
            }
        }
    }

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

    // Trap
    auto textureWhite2 = NFSEngine::Texture::Create("assets/textures/white.png");
    m_HazardCube = m_Scene->CreateGameObject("Hazard_Cube");
    m_HazardCube->GetTransform()->SetPosition(glm::vec3(5.0f, -1.0f, 5.0f));
    m_HazardCube->GetTransform()->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
    auto& hazardMesh = m_HazardCube->AddComponent<NFSEngine::CubeMesh>(m_HazardShader, textureWhite2);
    (void)hazardMesh;
    auto& hazardCol = m_HazardCube->AddComponent<NFSEngine::BoxCollider3DComponent>();
    hazardCol.Size = glm::vec3(1.5f, 1.5f, 1.5f);
    hazardCol.IsTrigger = true;
    auto& hazard = m_HazardCube->AddComponent<NFSEngine::HazardComponent>();
    hazard.SetTarget(m_Player);
    hazard.HitCooldown = 1.5f;
    hazard.OnPlayerHit = [this]() {
        if (m_UILayer) m_UILayer->LoseHeart();
    };

    // Audio
    NFSEngine::AudioEngine::Init();
    m_Sequencer.Start(120.0f);

    NFSEngine::GameObject* pianoObj = m_Scene->CreateGameObject("PianoTest");
    auto& audioComp = pianoObj->AddComponent<NFSEngine::AudioPatternComponent>();
    audioComp.LoadPattern("assets/audio/patterns/PianoPattern1.json", &m_Sequencer);
    m_TestAudioComp = &audioComp;

    // PianoObject
    NFSEngine::GameObject* pianoManagerObj = m_Scene->CreateGameObject("PianoManager");
    auto& pianoLogic = pianoManagerObj->AddComponent<InteractivePiano>();
    pianoLogic.LoadPiano("assets/audio/sounds/piano01.ogg");

    NFSEngine::GameObject* pianoBase = m_Scene->CreateGameObject("PianoBase");
    pianoBase->AddComponent<NFSEngine::CubeMesh>(m_Shader, textureBlack);
    pianoBase->GetTransform()->SetPosition(glm::vec3(44.f, -2.f, 0.f));
    pianoBase->GetTransform()->SetScale(glm::vec3(12.f, 1.f, 5.f));
    auto& pianoCol = pianoBase->AddComponent<NFSEngine::BoxCollider3DComponent>();
    pianoCol.Size = glm::vec3(12.f, 1.f, 5.f);

    for (int i = 0; i < 7; i++) {
        std::string keyName = "PianoKey" + std::to_string(i);
        NFSEngine::GameObject* keyObj = m_Scene->CreateGameObject(keyName);

        keyObj->AddComponent<NFSEngine::CubeMesh>(m_Shader, textureWhite);
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
        if (auto* spotLight = go->GetComponent<NFSEngine::SpotLight>()) m_CachedSpotLight = spotLight;
        if (auto* pointLight = go->GetComponent<NFSEngine::PointLight>()) m_CachedPointLights.push_back(pointLight);
        if (auto* mover = go->GetComponent<RhythmMover>()) m_CachedRhythmMovers.push_back(mover);
        if (auto* pianoKey = go->GetComponent<PianoKeyTrigger>()) m_CachedPianoKeys.push_back(pianoKey);
    }
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    m_Sequencer.Update((float)deltaTime);
    m_DeltaTime = deltaTime;
    m_Scene->OnUpdate(deltaTime);

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
    NFSEngine::Renderer::GetAPI().SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
    NFSEngine::Renderer::GetAPI().Clear();

    if (m_CachedCamera) {
        auto bindLightsAndCamera = [&](std::shared_ptr<NFSEngine::Shader> currentShader) {
            currentShader->Bind();
            currentShader->SetVec3("viewPos", m_CachedCamera->GetOwner()->GetTransform()->GetPosition());

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
                if (lightIndex >= 4) break;
            }
            currentShader->SetInt("activePointLights", lightIndex);

            if (m_CachedSpotLight) {
                currentShader->SetVec3("spotLight.position", m_CachedSpotLight->GetTransform()->GetPosition());
                currentShader->SetVec3("spotLight.direction", m_CachedSpotLight->Direction);
                currentShader->SetVec3("spotLight.color", m_CachedSpotLight->Color);
                currentShader->SetFloat("spotLight.intensity", m_CachedSpotLight->Intensity);
                currentShader->SetFloat("spotLight.cutOff", m_CachedSpotLight->CutOff);
                currentShader->SetFloat("spotLight.outerCutOff", m_CachedSpotLight->OuterCutOff);
                currentShader->SetFloat("spotLight.constant", m_CachedSpotLight->Constant);
                currentShader->SetFloat("spotLight.linear", m_CachedSpotLight->Linear);
                currentShader->SetFloat("spotLight.quadratic", m_CachedSpotLight->Quadratic);
            }
        };

        bindLightsAndCamera(m_Shader);
        bindLightsAndCamera(m_AudioShader);
        bindLightsAndCamera(m_ToonShader);

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

        NFSEngine::Renderer::BeginScene(m_CachedCamera->GetViewMatrix(), m_CachedCamera->GetProjectionMatrix());
        NFSEngine::Renderer::DrawSkybox(m_Skybox, m_SkyboxShader);
        if (m_Scene) m_Scene->OnRender();
        NFSEngine::Renderer::EndScene();
    }
}

void LayerExample::OnImGuiRender() {
    ImGui::Begin("Diagnostic window");

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
    if (m_CachedCameraController) {
        m_CachedCameraController->OnEvent(e);
    }

    for (auto* mover : m_CachedRhythmMovers) {
        mover->OnEvent(e);
    }
}
