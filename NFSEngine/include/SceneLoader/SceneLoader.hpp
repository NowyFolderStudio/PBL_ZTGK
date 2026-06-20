#pragma once

#include <vector>
#include <memory>
#include "Core/Scene.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include <queue>
#include <future>

namespace NFSEngine {
    class SceneLoader {
    public:
        void RegisterLoader(std::unique_ptr<IComponentLoader> loader);
        void InitDefaultLoaders();
        void LoadSceneAsync(Scene* currentScene, const std::string& filepath);
        void LoadScene(Scene* currentScene, const std::string& filepath);
        void ProcessLoading();

        bool IsLoading() const { return m_CurrentState != LoadingState::Idle; }
        float GetProgress() const;

    private:
        std::vector<std::unique_ptr<IComponentLoader>> m_Loaders;

        enum class LoadingState {
            Idle,
            ParsingJSON,
            InstantiatingObjects,
            Finalizing
        };
        LoadingState m_CurrentState = LoadingState::Idle;

        std::future<nlohmann::json> m_JsonFuture;
        std::queue<nlohmann::json> m_ObjectsToLoadQueue;

        Scene* m_TargetScene = nullptr;
        size_t m_TotalObjectsToLoad = 0;
        std::unordered_map<int, GameObject*> m_InstanceMap;
        nlohmann::json m_LoadedJsonDocument;
    };
} // namespace NFSEngine