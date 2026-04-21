#include "Panels/DiagnosticPanel.hpp"
#include "NFSEngine.h"

#include <imgui.h>

namespace NFSEngine {

    void DiagnosticPanel::OnUpdate(DeltaTime deltaTime) { m_DeltaTime = deltaTime; }

    void DiagnosticPanel::OnImGuiRender() {
        ImGui::Begin("Diagnostic window");

        float averageFps = ImGui::GetIO().Framerate;
        float averageFrameTime = 1000.0f / averageFps;
        float currentFps = m_DeltaTime.GetFPS();
        float currentFrameTime = m_DeltaTime.GetMilliseconds();

        ImGui::Text("(Average) FPS: %.1f", averageFps);
        ImGui::Text("(Average) Frame Time: %.3f ms", averageFrameTime);

        ImGui::Text("(Current) FPS: %.1f", currentFps);
        ImGui::Text("(Current) Frame Time: %.3f ms", currentFrameTime);

        ImGui::Separator();

        ImGui::Text("GPU: %.3f ms", NFSEngine::Renderer::GetGPUTime());

        ImGui::Separator();

        auto stats = NFSEngine::Renderer::GetStats();
        ImGui::Text("Renderer Stats:");
        ImGui::Text("Draw Calls: %u", stats.drawCalls);
        ImGui::Text("Triangle count: %u", stats.triangleCount);
        ImGui::Text("State changes: %u", stats.stateChanges);

        static float values[90] = { 0 };
        static int values_offset = 0;
        values[values_offset] = currentFrameTime;
        values_offset = (values_offset + 1) % 90;
        ImGui::PlotLines("History of frameTime (ms)", values, 90, values_offset, nullptr, 0.0f, 33.3f, ImVec2(0, 80));

        ImGui::End();
    }
} // namespace NFSEngine