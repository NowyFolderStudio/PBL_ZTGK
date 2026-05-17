#pragma once

#include <vector>
#include <memory>
#include "Core/Scene.hpp"
#include "SceneLoader/IComponentLoader.hpp"

namespace NFSEngine {
    class SceneLoader {
    public:
        void RegisterLoader(std::unique_ptr<IComponentLoader> loader);
        void InitDefaultLoaders();
        void LoadScene(Scene* currentScene, const std::string& filepath);

    private:
        std::vector<std::unique_ptr<IComponentLoader>> m_Loaders;
    };
} // namespace NFSEngine