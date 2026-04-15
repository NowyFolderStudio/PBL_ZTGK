#pragma once

#include "UIComponents.hpp"

namespace NFSEngine {
    class UIRenderer {
    public:
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