#pragma once
#include <json/json.hpp>
#include "Core/GameObject.hpp"
#include <unordered_map>

namespace NFSEngine {
    class IComponentLoader {
    public:
        virtual ~IComponentLoader() = default;

        virtual void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) = 0;

        virtual void PostLoad(const nlohmann::json& j_obj, GameObject* targetObj,
                              const std::unordered_map<int, GameObject*>& instanceMap) { }
    };
} // namespace NFSEngine