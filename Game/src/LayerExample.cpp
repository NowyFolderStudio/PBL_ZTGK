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

#include "Components/PhysicsComponents.hpp"

#include <imgui.h>

LayerExample::LayerExample() {
    m_MyCube = nullptr;
    m_MovingCube = nullptr;
    m_Scene = nullptr;
}

LayerExample::~LayerExample() {
    if (m_MyCube) delete m_MyCube;
}

void LayerExample::OnAttach() {
    Init();
    m_Scene = std::make_unique<NFSEngine::Scene>();
    m_HierarchyPanel = std::make_unique<NFSEngine::SceneHierarchyPanel>(m_Scene.get());

    auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");
    auto texture2 = NFSEngine::Texture::Create("assets/textures/sample.png");

    m_MovingCube = m_Scene->CreateGameObject("Player_Moving");
    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    m_MovingCube->AddComponent<CubeControl>();

    // Physics tests on cube
    m_MovingCube->AddComponent<NFSEngine::RigidBody3DComponent>();
    m_MovingCube->AddComponent<NFSEngine::BoxCollider3DComponent>();

    // Sample floor object for physics test
    m_Floor = m_Scene->CreateGameObject("Floor");
    m_Floor->GetTransform()->SetPosition({ 0.0f, -2.0f, 0.0f });

    m_Floor->AddComponent<NFSEngine::CubeMesh>(shader, texture2);
    m_Floor->AddComponent<NFSEngine::BoxCollider3DComponent>();

    m_Floor->GetComponent<NFSEngine::BoxCollider3DComponent>()->Size = glm::vec3(20.0f, 1.0f, 20.0f);
    m_Floor->GetTransform()->SetScale({ 20.0f, 1.0f, 20.0f });

    m_MovingCube2 = m_Scene->CreateGameObject("Static_Reference_Cube");
    m_MovingCube2->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    m_MovingCube2->GetTransform()->SetPosition({ -4.0f, -1.0f, 0.0f });
    m_MovingCube2->AddComponent<NFSEngine::BoxCollider3DComponent>();

    auto earthModel = std::make_shared<NFSEngine::Model>("assets/models/Earth/Sun.gltf");
    auto earthTexture = NFSEngine::Texture::Create("assets/models/Earth/2k_earth_daymap.jpg");

    NFSEngine::GameObject* earthObj = m_Scene->CreateGameObject("Earth");
    earthObj->GetComponent<NFSEngine::Transform>()->SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
    earthObj->AddComponent<NFSEngine::ModelComponent>(earthModel, shader, earthTexture);
    earthObj->AddComponent<NFSEngine::SphereCollider3DComponent>();

    NFSEngine::GameObject* cameraObj = m_Scene->CreateGameObject("MainCamera");
    cameraObj->AddComponent<NFSEngine::Camera>();
    auto& controller = cameraObj->AddComponent<NFSEngine::CameraController>();
    controller.SetTarget(m_MovingCube->GetTransform());

    const auto& gameObjects = m_Scene->GetAllGameObjects();
    for (const auto& go : gameObjects) {
        if (go.get() != m_MovingCube) {
            if (auto* control = go->GetComponent<CubeControl>()) {
                control->SetActive(false);
            }
        }
    }
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    m_DeltaTime = deltaTime;
    m_Scene->OnUpdate(deltaTime);
    Update();
    Render();
}

void LayerExample::OnRender() { Render(); }

void LayerExample::Init() { m_MyCube = new NFSEngine::Cube(); }

void LayerExample::Update() { }

void LayerExample::Render() {
    NFSEngine::Renderer::GetAPI().SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
    NFSEngine::Renderer::GetAPI().Clear();

    NFSEngine::Camera* mainCamera = nullptr;
    for (auto& go : m_Scene->GetAllGameObjects()) {
        if (auto* cam = go->GetComponent<NFSEngine::Camera>()) {
            mainCamera = cam;
            break;
        }
    }

    if (mainCamera) {
        NFSEngine::Renderer::BeginScene(mainCamera->GetViewMatrix(), mainCamera->GetProjectionMatrix());

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
    auto& gameObjects = m_Scene->GetAllGameObjects();
    for (auto& go : gameObjects) {
        if (auto* controller = go->GetComponent<NFSEngine::CameraController>()) controller->OnEvent(e);
    }
}
