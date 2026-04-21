#pragma once

#include "NFSEngine.h"

namespace NFSEngine {

    class EditorPanel;
    class ViewportPanel;

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(DeltaTime deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;

    private:
        std::shared_ptr<Scene> m_ActiveScene;
        std::vector<std::unique_ptr<EditorPanel>> m_Panels;
        std::shared_ptr<Framebuffer> m_Framebuffer;
        ViewportPanel* m_ViewportPanel;
    };

} // namespace NFSEngine