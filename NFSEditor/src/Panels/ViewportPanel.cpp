#include "Panels/ViewportPanel.hpp"
#include <imgui.h>

namespace NFSEngine {

    void ViewportPanel::OnImGuiRender() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
        ImGui::Begin("Viewport");

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        if (m_TextureID != 0) {
            ImGui::Image((void*)(intptr_t)m_TextureID, ImVec2 { m_ViewportSize.x, m_ViewportSize.y }, ImVec2 { 0, 1 },
                         ImVec2 { 1, 0 });
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

} // namespace NFSEngine