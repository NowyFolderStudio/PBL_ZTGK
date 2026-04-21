#include "NFSEngine.h"
#include "Core/EntryPoint.hpp"
#include "Layers/EditorLayer.hpp"

namespace NFSEngine {
    class NFSEditor : public Application {
    public:
        NFSEditor(const ApplicationConfig& config)
            : Application(config) {
            PushLayer(new EditorLayer());
        }

        ~NFSEditor() { }
    };

    Application* CreateApplication() {
        ApplicationConfig config;
        config.WindowTitle = "NFS Engine - Editor";
        config.WindowWidth = 1600;
        config.WindowHeight = 900;
        config.VSync = true;

        return new NFSEditor(config);
    }
} // namespace NFSEngine