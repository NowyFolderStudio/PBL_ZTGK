#pragma once

#include "Panels/EditorPanel.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"

namespace NFSEngine {

    class SceneHierarchyPanel : public EditorPanel {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(Scene* context);
        void SetContext(Scene* context);
        void OnImGuiRender() override;
        GameObject* GetSelectedGameObject() const { return m_SelectionContext; }

    private:
        void DrawGameObjectNode(GameObject* obj);
        void DrawComponents(GameObject* obj);

        Scene* m_Context = nullptr;
        GameObject* m_SelectionContext = nullptr;
    };

} // namespace NFSEngine