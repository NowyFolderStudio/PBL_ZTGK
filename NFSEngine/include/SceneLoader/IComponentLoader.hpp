#pragma once
#include <json/json.hpp>
#include "Core/GameObject.hpp"

namespace NFSEngine {
    class IComponentLoader {
    public:
        virtual ~IComponentLoader() = default;

        virtual void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) = 0;
    };
} // namespace NFSEngine