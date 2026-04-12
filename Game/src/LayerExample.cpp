#include "LayerExample.hpp"
#include "Components/CubeMesh.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include <memory>
#include "Components/CubeControl.hpp"
#include "Renderer/Renderer.hpp"

#include <imgui.h>

LayerExample::LayerExample()
{
    m_MyShader = nullptr;
    m_MyTexture = nullptr;
    m_MyCube = nullptr;
    m_MovingCube = nullptr;
    m_Scene = nullptr;
}

LayerExample::~LayerExample()
{
    if (m_MyCube) delete m_MyCube;
}

void LayerExample::OnAttach()
{
    Init();
    m_Scene = std::make_unique<NFSEngine::Scene>();
    m_MovingCube = m_Scene->CreateGameObject("movingCube");
    m_MovingCube2 = m_Scene->CreateGameObject("movingCube2");
    m_HierarchyPanel = std::make_unique<NFSEngine::SceneHierarchyPanel>(m_Scene.get());

    auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");

    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    m_MovingCube->AddComponent<CubeControl>();
    m_MovingCube2->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    m_MovingCube2->AddComponent<CubeControl>();
    m_MovingCube2->GetTransform()->SetParent(m_MovingCube->GetTransform(), true);
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime)
{
    m_DeltaTime = deltaTime;
    m_Scene->OnUpdate(deltaTime);
    Update();
    Render();
}

void LayerExample::OnRender() { Render(); }

void LayerExample::Init()
{
    m_MyShader = NFSEngine::Shader::Create("MainShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    m_MyTexture = NFSEngine::Texture::Create("assets/textures/cat.png");
    m_MyCube = new NFSEngine::Cube();
}

void LayerExample::Update() { }

void LayerExample::Render()
{

    NFSEngine::Renderer::GetAPI().SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
    NFSEngine::Renderer::GetAPI().Clear();

    glm::mat4 view3D = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection3D = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    NFSEngine::Renderer::BeginScene(view3D, projection3D);

    if (m_Scene)
    {
        m_Scene->OnRender();
    }

    NFSEngine::Renderer::EndScene();
}

void LayerExample::OnImGuiRender()
{
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

void LayerExample::OnEvent(NFSEngine::Event& e) { }