#pragma once
#include "Panels/EditorPanel.hpp"

namespace NFSEngine {

    class DiagnosticPanel : public EditorPanel {
    public:
        DiagnosticPanel() = default;
        void OnUpdate(DeltaTime deltaTime) override;
        void OnImGuiRender() override;

    private:
        DeltaTime m_DeltaTime;
    };

} // namespace NFSEngine