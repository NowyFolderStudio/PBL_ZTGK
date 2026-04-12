#include "LayerExample.hpp"
#include "Core/Application.hpp"
#include "Components/CubeMesh.hpp"
#include "Components/Transform.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "GLFW/glfw3.h"
#include <cstddef>
#include <memory>
#include "Components/CubeControl.hpp"
#include "Renderer/Renderer.hpp"

#include <imgui.h>

#include "Events/ApplicationEvent.hpp"

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
    m_MovingCube->AddComponent<NFSEngine::Transform>();

    auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");

    m_MovingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);
    m_MovingCube->AddComponent<CubeControl>();
}

void LayerExample::OnDetach() { }

void LayerExample::OnUpdate(NFSEngine::DeltaTime deltaTime)
{
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

    float fps = ImGui::GetIO().Framerate;
    float frameTime = 1000.0f / fps;

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", frameTime);

    ImGui::Separator();

    ImGui::Text("Etapy Pipeline'u:");
    ImGui::Text("GPU: %.3f ms", NFSEngine::Renderer::GetGPUTime());

    // Możesz tu też dodać czas CPU z Profilera, jeśli go przechowujesz
    // ImGui::Text("CPU (Przygotowanie): %.3f ms", m_LastCPURenderTime);

    ImGui::Separator();

    auto stats = NFSEngine::Renderer::GetStats();
    ImGui::Text("Renderer Stats:");
    ImGui::Text("Draw Calls: %u", stats.drawCalls);
    ImGui::Text("Triangle count: %u", stats.triangleCount);
    ImGui::Text("State changes: %u", stats.stateChanges);

    static float values[90] = { 0 };
    static int values_offset = 0;
    values[values_offset] = frameTime;
    values_offset = (values_offset + 1) % 90;
    ImGui::PlotLines("History of frameTime (ms)", values, 90, values_offset, nullptr, 0.0f, 33.3f, ImVec2(0, 80));

    ImGui::End();
}

void LayerExample::OnEvent(NFSEngine::Event& e) { }