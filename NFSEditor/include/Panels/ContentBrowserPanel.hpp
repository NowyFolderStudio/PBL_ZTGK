#pragma once

#include "Panels/EditorPanel.hpp"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

namespace NFSEngine {
    class ContentBrowserPanel : public EditorPanel {
    public:
        ContentBrowserPanel() { m_CurrentDirectory = std::filesystem::current_path() / "assets"; }

        void OnImGuiRender() override {
            ImGui::Begin("Content Browser");

            if (m_CurrentDirectory != std::filesystem::path("assets")) {
                if (ImGui::Button("<- Back")) {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                }
            }

            static float padding = 16.0f;
            static float cellSize = 64.0f;
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int)(panelWidth / (cellSize + padding));
            columnCount = std::max(columnCount, 1);

            ImGui::Columns(columnCount, 0, false);

            for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
                const auto& path = directoryEntry.path();
                auto relativePath = std::filesystem::relative(path, "assets");
                std::string filenameString = relativePath.filename().string();

                ImGui::Button(filenameString.c_str(), { cellSize, cellSize });

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory()) m_CurrentDirectory /= path.filename();
                }
                ImGui::TextWrapped("%s", filenameString.c_str());
                ImGui::NextColumn();
            }

            ImGui::Columns(1);
            ImGui::End();
        }

    private:
        std::filesystem::path m_CurrentDirectory;
    };
} // namespace NFSEngine