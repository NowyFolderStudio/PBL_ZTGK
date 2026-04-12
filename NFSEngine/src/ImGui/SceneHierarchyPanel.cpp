#include "ImGui/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace NFSEngine
{

SceneHierarchyPanel::SceneHierarchyPanel(Scene* context) { SetContext(context); }

void SceneHierarchyPanel::SetContext(Scene* context)
{
    m_Context = context;
    m_SelectionContext = nullptr;
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    if (m_Context)
    {
        for (auto& gameObject : m_Context->GetAllGameObjects())
        {
            if (gameObject->GetTransform()->GetParent() == nullptr)
            {
                DrawGameObjectNode(gameObject.get());
            }
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) m_SelectionContext = nullptr;
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext)
    {
        DrawComponents(m_SelectionContext);
    }
    ImGui::End();
}

void SceneHierarchyPanel::DrawGameObjectNode(GameObject* obj)
{
    auto& name = obj->name;

    ImGuiTreeNodeFlags flags = ((m_SelectionContext == obj) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (obj->GetTransform()->GetChildCount() == 0) flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)obj, flags, "%s", name.c_str());

    if (ImGui::IsItemClicked())
    {
        m_SelectionContext = obj;
    }

    if (opened)
    {
        Transform* transform = obj->GetTransform();
        for (int i = 0; i < transform->GetChildCount(); i++)
        {
            DrawGameObjectNode(transform->GetChild(i)->GetOwner());
        }
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::DrawComponents(GameObject* obj)
{
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, obj->name.c_str());

    if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
    {
        obj->name = std::string(buffer);
    }

    ImGui::Separator();

    for (auto& component : obj->GetComponents())
    {
        if (ImGui::CollapsingHeader(component->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {

            ImGui::PushID(component.get()); // Zapobiega konfliktom ID w ImGui
            component->OnImGuiRender(); // Wywołanie wirtualnej metody!
            ImGui::PopID();
        }
    }
}

}