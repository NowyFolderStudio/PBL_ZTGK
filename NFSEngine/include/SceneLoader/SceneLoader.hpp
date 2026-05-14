#pragma once

#include "Core/Scene.hpp"

namespace NFSEngine {
    class SceneLoader {
    public:
        static void LoadScene(Scene* currentScene, const std::string& filepath);
    };
} // namespace NFSEngine