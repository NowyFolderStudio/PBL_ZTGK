#include "Layers/LayerExample.hpp"
#include "Layers/UILayer.hpp"
#include "InputActions.hpp"
#include "NFSEngine.h"
#include "Core/EntryPoint.hpp"

#include "GameManager.hpp"

class GameManagerLayer : public NFSEngine::Layer {
public:
    virtual void OnUpdate(NFSEngine::DeltaTime dt) override { GameManager::Get().ProcessStateChange(); }
};

class GameApp : public NFSEngine::Application {
public:
    GameApp(const NFSEngine::ApplicationConfig& config)
        : NFSEngine::Application(config) {
        NFSEngine::RegisterInputActions();

        PushLayer(new GameManagerLayer());

        GameManager::Get().Init();
    }

    ~GameApp() { }
};

NFSEngine::Application* NFSEngine::CreateApplication() {
    ApplicationConfig config;
    config.WindowTitle = "NFS Game Window";
    config.WindowWidth = 1000;
    config.WindowHeight = 600;

    return new GameApp(config);
}