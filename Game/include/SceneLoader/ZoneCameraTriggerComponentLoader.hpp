#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/ZoneCameraTriggerComponent.hpp"

using namespace NFSEngine;

class ZoneCameraTriggerComponentLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "ZoneCameraTrigger") {
                targetObj->AddComponent<ZoneCameraTriggerComponent>();
                break;
            }
        }
    }
};
