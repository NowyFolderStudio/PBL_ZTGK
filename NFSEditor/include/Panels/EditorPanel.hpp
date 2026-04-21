#pragma once

#include "NFSEngine.h"

namespace NFSEngine {
    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUpdate(DeltaTime deltaTime) { }
        virtual void OnImGuiRender() = 0;

        bool IsEnabled() const { return m_Enabled; };

        void SetEnabled(bool enabled) { m_Enabled = enabled; }

    private:
        bool m_Enabled = true;
    };
} // namespace NFSEngine