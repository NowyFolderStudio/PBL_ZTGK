#include <glad/glad.h>
#include "Layers/EditorLayer.hpp"

#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/DiagnosticPanel.hpp"
#include "Panels/ViewportPanel.hpp"

#include <imgui.h>

namespace NFSEngine {
    EditorLayer::EditorLayer()
        : Layer("EditorLayer") { }

    void EditorLayer::OnAttach() {
        FramebufferSpecification fbSpec;
        fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
        fbSpec.width = 1280;
        fbSpec.height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = std::make_shared<Scene>();

        auto shader = NFSEngine::Shader::Create("BasicShader", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
        auto texture = NFSEngine::Texture::Create("assets/textures/cat.png");
        auto texture2 = NFSEngine::Texture::Create("assets/textures/sample.png");

        auto m_MovingCube = m_ActiveScene->CreateGameObject("Player_Moving");
        m_MovingCube->AddComponent<NFSEngine::CubeMesh>(shader, texture);

        auto* camera = m_ActiveScene->CreateGameObject("Main Camera");
        NFSEngine::GameObject* cameraObj = m_ActiveScene->CreateGameObject("MainCamera");
        cameraObj->AddComponent<NFSEngine::Camera>();

        m_Panels.push_back(std::make_unique<SceneHierarchyPanel>(m_ActiveScene.get()));
        m_Panels.push_back(std::make_unique<ContentBrowserPanel>());
        m_Panels.push_back(std::make_unique<DiagnosticPanel>());

        auto viewPortPanel = std::make_unique<ViewportPanel>();

        m_ViewportPanel = viewPortPanel.get();

        m_Panels.push_back(std::move(viewPortPanel));
    }

    void EditorLayer::OnDetach() { }

    void EditorLayer::OnUpdate(DeltaTime deltaTime) {
        glm::vec2 viewportSize = m_ViewportPanel->GetViewportSize();

        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f
            && (viewportSize.x != m_Framebuffer->GetSpecification().width
                || viewportSize.y != m_Framebuffer->GetSpecification().height)) {
            m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        }

        if (m_ActiveScene) {
            m_ActiveScene->OnUpdate(deltaTime);
        }

        NFSEngine::Camera* mainCamera = nullptr;
        for (auto& go : m_ActiveScene->GetAllGameObjects()) {
            if (auto* cam = go->GetComponent<NFSEngine::Camera>()) {
                mainCamera = cam;
                break;
            }
        }
        m_Framebuffer->Bind();
        NFSEngine::Renderer::GetAPI().SetClearColor({ 0.2f, 0.1f, 0.1f, 1.0f });
        NFSEngine::Renderer::GetAPI().Clear();
        if (m_ActiveScene) {
            NFSEngine::Renderer::BeginScene(mainCamera->GetViewMatrix(), mainCamera->GetProjectionMatrix());

            if (m_ActiveScene) m_ActiveScene->OnRender();

            NFSEngine::Renderer::EndScene();
        }
        m_Framebuffer->Unbind();

        // 6. Aktualizacja logiki paneli
        for (auto& panel : m_Panels) {
            if (panel->IsEnabled()) panel->OnUpdate(deltaTime);
        }
    }

    void EditorLayer::OnImGuiRender() {
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0); // 0, bo to nasz pierwszy kolor (RGBA8)
        m_ViewportPanel->SetTextureID(textureID);

        for (auto& panel : m_Panels) {
            if (panel->IsEnabled()) panel->OnImGuiRender();
        }
    }

    void EditorLayer::OnEvent(Event& e) { }
} // namespace NFSEngine