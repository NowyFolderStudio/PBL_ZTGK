#include "Layers/GameLayer.hpp"
#include "SceneLoader/SceneLoader.hpp"
#include "SceneLoader/CoinComponentLoader.hpp"
#include "Components/Camera.hpp"

// It doesn't work now

GameLayer::GameLayer(const std::string& levelPath, UILayer* uiLayer)
    : m_LevelFilePath(levelPath)
    , m_UILayer(uiLayer) { }

GameLayer::~GameLayer() { }

void GameLayer::OnAttach() {
    m_Scene = std::make_unique<NFSEngine::Scene>();

    NFSEngine::SceneLoader sceneLoader;
    sceneLoader.InitDefaultLoaders();
    sceneLoader.RegisterLoader(std::make_unique<CoinComponentLoader>());
    sceneLoader.LoadScene(m_Scene.get(), m_LevelFilePath);

    NFSEngine::AudioEngine::Init();

    for (const auto& go : m_Scene->GetAllGameObjects()) {
        if (auto* cam = go->GetComponent<NFSEngine::Camera>()) m_CachedCamera = cam;
    }
}
void GameLayer::OnDetach() { }
void GameLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) { }
void GameLayer::OnRender() { }
void GameLayer::OnEvent(NFSEngine::Event& e) { }
