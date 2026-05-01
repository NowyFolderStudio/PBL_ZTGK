#include "LayerExample.hpp"
#include "Components/CubeMesh.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include <memory>
#include "Components/CubeControl.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Model.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/CameraController.hpp"
#include "Components/Camera.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Components/AudioComponent.hpp"
#include "Components/PointLight.hpp"
#include "Components/DirectionalLight.hpp"
#include "Components/SpotLight.hpp"

#include "Components/PhysicsComponents.hpp"

#include <imgui.h>

LayerExample::LayerExample() {
    m_Player = nullptr;
    m_MyCube = nullptr;
    m_MovingCube = nullptr;
    m_Scene = nullptr;
}

LayerExample::~LayerExample() {
    if (m_MyCube) delete m_MyCube;
    NFSEngine::AudioEngine::Shutdown();
}

void LayerExample::OnAttach() {
    Init();
    m_Scene = std::make_unique<NFSEngine::Scene>();
    m_HierarchyPanel = std::make_unique<NFSEngine::SceneHierarchyPanel>(m_Scene.get());

    m_Shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/lightShader.frag");
    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");
    auto texture2 = NFSEngine::Texture::Create("assets/textures/sample.png");

    m_MovingCube = m_Scene->CreateGameObject("Player_Moving");
    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture);

    // Physics tests on cube

    m_MovingCube->AddComponent<NFSEngine::BoxCollider3DComponent>();

    auto capsuleModel = std::make_shared<NFSEngine::Model>("assets/models/Capsule/capsule.obj");

    m_Player = m_Scene->CreateGameObject("Player");
    m_Player->GetComponent<NFSEngine::Transform>()->SetPosition(glm::vec3(2.0f, 3.0f, 5.0f));
    m_Player->AddComponent<NFSEngine::ModelComponent>(capsuleModel, m_Shader, texture);
    m_Player->AddComponent<NFSEngine::CapsuleCollider3DComponent>();
    m_Player->AddComponent<NFSEngine::RigidBody3DComponent>();
    m_Player->AddComponent<CubeControl>();

    auto cylinderModel = std::make_shared<NFSEngine::Model>("assets/models/Cylinder/cylinder.obj");

    NFSEngine::GameObject* cylinderObj = m_Scene->CreateGameObject("Static_Cylinder");
    cylinderObj->AddComponent<NFSEngine::CylinderCollider3DComponent>();
    cylinderObj->AddComponent<NFSEngine::ModelComponent>(cylinderModel, m_Shader, texture2);
    cylinderObj->GetTransform()->SetPosition({4.0f, 0.0f, 1.0f});

    // Sample floor object for physics test
    m_Floor = m_Scene->CreateGameObject("Floor");
    m_Floor->GetTransform()->SetPosition({0.0f, -2.0f, 0.0f});

    m_Floor->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture2);
    m_Floor->AddComponent<NFSEngine::BoxCollider3DComponent>();

    m_Floor->GetComponent<NFSEngine::BoxCollider3DComponent>()->Size = glm::vec3(20.0f, 1.0f, 20.0f);
    m_Floor->GetTransform()->SetScale({20.0f, 1.0f, 20.0f});

    NFSEngine::GameObject* lightObj = m_Scene->CreateGameObject("PointLight_1");
    lightObj->GetTransform()->SetPosition({0.0f, 2.0f, 2.0f});
    auto& lightComp = lightObj->AddComponent<NFSEngine::PointLight>();
    lightComp.Color = {1.0f, 0.8f, 0.5f};
    lightComp.Intensity = 3.f;

    NFSEngine::GameObject* sunObj = m_Scene->CreateGameObject("Sun");
    auto& sunComp = sunObj->AddComponent<NFSEngine::DirectionalLight>();
    sunComp.Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    sunComp.Color = glm::vec3(1.0f, 0.9f, 0.8f);
    sunComp.Intensity = 0.8f;

    NFSEngine::GameObject* spotObj = m_Scene->CreateGameObject("MainSpotLight");
    spotObj->GetTransform()->SetPosition({0.0f, 1.5f, 0.0f});

    auto& spotComp = spotObj->AddComponent<NFSEngine::SpotLight>();
    spotComp.Color = {1.0f, 1.0f, 1.0f};
    spotComp.Direction = {0.0f, -1.0f, -0.5f};
    spotComp.Intensity = 5.0f;

    m_MovingCube2 = m_Scene->CreateGameObject("Static_Reference_Cube");
    m_MovingCube2->AddComponent<NFSEngine::CubeMesh>(m_Shader, texture);
    m_MovingCube2->GetTransform()->SetPosition({-4.0f, -1.0f, 0.0f});
    m_MovingCube2->AddComponent<NFSEngine::BoxCollider3DComponent>();

    auto earthModel = std::make_shared<NFSEngine::Model>("assets/models/Earth/Sun.gltf");
    auto earthTexture = NFSEngine::Texture::Create("assets/models/Earth/2k_earth_daymap.jpg");

    NFSEngine::GameObject* earthObj = m_Scene->CreateGameObject("Earth");
    earthObj->GetComponent<NFSEngine::Transform>()->SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
    earthObj->AddComponent<NFSEngine::ModelComponent>(earthModel, m_Shader, earthTexture);
    earthObj->AddComponent<NFSEngine::SphereCollider3DComponent>();

    NFSEngine::GameObject* cameraObj = m_Scene->CreateGameObject("MainCamera");
    cameraObj->AddComponent<NFSEngine::Camera>();
    auto& controller = cameraObj->AddComponent<NFSEngine::CameraController>();
    controller.SetTarget(m_Player->GetTransform());

    std::vector<std::string> faces = {
        "assets/textures/skybox/testSkybox/px.png", // 1. Right (+X)
        "assets/textures/skybox/testSkybox/nx.png", // 2. Left (-X)
        "assets/textures/skybox/testSkybox/py.png", // 3. Top (+Y)
        "assets/textures/skybox/testSkybox/ny.png", // 4. Bottom (-Y)
        "assets/textures/skybox/testSkybox/pz.png", // 5. Front (+Z)
        "assets/textures/skybox/testSkybox/nz.png" // 6. Back (-Z)
    };
    m_Skybox = NFSEngine::Skybox::Create(faces);
    m_SkyboxShader = NFSEngine::Shader::Create("Skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    NFSEngine::Renderer::DrawSkybox(m_Skybox, m_SkyboxShader);

    const auto& gameObjects = m_Scene->GetAllGameObjects();
    for (const auto& go : gameObjects) {
        if (go.get() != m_Player) {
            if (auto* control = go->GetComponent<CubeControl>()) {
                control->SetActive(false);
            }
        }
    }

    NFSEngine::AudioEngine::Init();
    m_Sequencer.Start(120.0f);

    NFSEngine::GameObject* pianoObj = m_Scene->CreateGameObject("PianoTest");
    auto& audioComp = pianoObj->AddComponent<NFSEngine::AudioComponent>();
    audioComp.LoadPattern("assets/audio/patterns/PianoPattern1.json", &m_Sequencer);

    m_TestAudioComp = &audioComp;
}

void LayerExample::OnDetach() {}

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    m_Sequencer.Update((float)deltaTime);
    m_DeltaTime = deltaTime;
    m_Scene->OnUpdate(deltaTime);

    if (m_TestAudioComp) {
        m_TestAudioComp->OnUpdate(deltaTime);
    }

    Update();
    Render();
}

void LayerExample::OnRender() { Render(); }

void LayerExample::Init() { m_MyCube = new NFSEngine::Cube(); }

void LayerExample::Update() {}

void LayerExample::Render() {
    NFSEngine::Renderer::GetAPI().SetClearColor({0.2f, 0.1f, 0.1f, 1.0f});
    NFSEngine::Renderer::GetAPI().Clear();

    NFSEngine::Camera* mainCamera = nullptr;
    for (auto& go : m_Scene->GetAllGameObjects()) {
        if (auto* cam = go->GetComponent<NFSEngine::Camera>()) {
            mainCamera = cam;
            break;
        }
    }

    if (mainCamera) {
        m_Shader->Bind();

        m_Shader->SetVec3("viewPos", mainCamera->GetOwner()->GetTransform()->GetPosition());

        for (auto& go : m_Scene->GetAllGameObjects()) {
            if (auto* dirLight = go->GetComponent<NFSEngine::DirectionalLight>()) {
                m_Shader->SetVec3("dirLight.direction", dirLight->Direction);
                m_Shader->SetVec3("dirLight.color", dirLight->Color);
                m_Shader->SetFloat("dirLight.intensity", dirLight->Intensity);
                break;
            }
        }

        int lightIndex = 0;
        for (auto& go : m_Scene->GetAllGameObjects()) {
            if (auto* light = go->GetComponent<NFSEngine::PointLight>()) {
                std::string base = "pointLights[" + std::to_string(lightIndex) + "].";

                m_Shader->SetVec3(base + "position", light->GetTransform()->GetPosition());
                m_Shader->SetVec3(base + "color", light->Color);
                m_Shader->SetFloat(base + "intensity", light->Intensity);
                m_Shader->SetFloat(base + "constant", light->Constant);
                m_Shader->SetFloat(base + "linear", light->Linear);
                m_Shader->SetFloat(base + "quadratic", light->Quadratic);

                lightIndex++;
                if (lightIndex >= 4) break;
            }
        }

        for (auto& go : m_Scene->GetAllGameObjects()) {
            if (auto* spot = go->GetComponent<NFSEngine::SpotLight>()) {
                m_Shader->SetVec3("spotLight.position", spot->GetTransform()->GetPosition());
                m_Shader->SetVec3("spotLight.direction", spot->Direction);
                m_Shader->SetVec3("spotLight.color", spot->Color);
                m_Shader->SetFloat("spotLight.intensity", spot->Intensity);
                m_Shader->SetFloat("spotLight.cutOff", spot->CutOff);
                m_Shader->SetFloat("spotLight.outerCutOff", spot->OuterCutOff);
                m_Shader->SetFloat("spotLight.constant", spot->Constant);
                m_Shader->SetFloat("spotLight.linear", spot->Linear);
                m_Shader->SetFloat("spotLight.quadratic", spot->Quadratic);
                break;
            }
        }
        m_Shader->SetInt("activePointLights", lightIndex);

        NFSEngine::Renderer::BeginScene(mainCamera->GetViewMatrix(), mainCamera->GetProjectionMatrix());

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

    static float values[90] = {0};
    static int values_offset = 0;
    values[values_offset] = currentFrameTime;
    values_offset = (values_offset + 1) % 90;
    ImGui::PlotLines("History of frameTime (ms)", values, 90, values_offset, nullptr, 0.0f, 33.3f, ImVec2(0, 80));

    ImGui::End();

    m_HierarchyPanel->OnImGuiRender();
}

void LayerExample::OnEvent(NFSEngine::Event& e) {
    auto& gameObjects = m_Scene->GetAllGameObjects();

    for (auto& go : gameObjects) {
        if (auto* controller = go->GetComponent<NFSEngine::CameraController>()) controller->OnEvent(e);
    }
}
