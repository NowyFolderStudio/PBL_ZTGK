#pragma once

#include "Panels/EditorPanel.hpp"
#include <glm/glm.hpp>
#include <stdint.h>

namespace NFSEngine {

    class ViewportPanel : public EditorPanel {
    public:
        ViewportPanel() = default;

        void OnUpdate(DeltaTime deltaTime) override { }
        void OnImGuiRender() override;

        void SetTextureID(uint32_t textureID) { m_TextureID = textureID; }

        glm::vec2 GetViewportSize() const { return m_ViewportSize; }

    private:
        uint32_t m_TextureID = 0;
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    };

} // namespace NFSEngine