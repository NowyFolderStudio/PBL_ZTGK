#include "LayerExample.hpp"
#include "UILayer.hpp"
#include "NFSEngine.h"
#include "Core/EntryPoint.hpp"

class GameApp : public NFSEngine::Application {
public:
    GameApp(const NFSEngine::ApplicationConfig& config)
        : NFSEngine::Application(config) {
        auto* uiLayer = new UILayer();
        PushLayer(new LayerExample(uiLayer));
        PushOverlay(uiLayer);
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