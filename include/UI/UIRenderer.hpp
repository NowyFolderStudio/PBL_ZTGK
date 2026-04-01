#pragma once

#include "UIComponents.hpp"

namespace NFSEngine {
    class UIRenderer {
    public:
        static void Init();

        static void Shutdown();

        static void SetProjection(float windowWidth, float windowHeight);

        static void DrawQuad(const RectTransform& transform, const ImageComponent& image);

    
    private:
        UIRenderer() = delete;

    };
};