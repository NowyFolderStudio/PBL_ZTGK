#pragma once

#include "UIComponents.hpp"

namespace NFSEngine {
    class UIRenderer {
    public:
        static constexpr float VIRTUAL_WIDTH = 1920.0f;
        static constexpr float VIRTUAL_HEIGHT = 1080.0f;

        static void Init();

        static void Shutdown();

        static void Begin();

        static void End();

        static void SetProjection(float windowWidth, float windowHeight);

        static void DrawQuad(const RectTransform& transform, const ImageComponent& image);

        static void DrawText(const RectTransform& transform, const TextComponent& text);

    private:
        UIRenderer() = delete;
    };
}; // namespace NFSEngine