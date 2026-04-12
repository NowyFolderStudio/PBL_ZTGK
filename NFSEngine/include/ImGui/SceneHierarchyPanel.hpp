#pragma once

#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"

#include <memory>

namespace NFSEngine
{

class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Scene* context);
    void SetContext(Scene* context);
    void OnImGuiRender();
    GameObject* GetSelectedGameObject() const { return m_SelectionContext; }

private:
    void DrawGameObjectNode(GameObject* obj);
    void DrawComponents(GameObject* obj);

    Scene* m_Context = nullptr;
    GameObject* m_SelectionContext = nullptr;
};

}