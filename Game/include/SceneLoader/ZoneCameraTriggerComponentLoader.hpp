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

                auto& zoneCameraComp = targetObj->AddComponent<ZoneCameraTriggerComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "customYaw") {
                        zoneCameraComp.CustomYaw = std::stof(propValue);
                    }
                    if (propName == "customPitch") {
                        zoneCameraComp.CustomPitch = std::stof(propValue);
                    }
                    if (propName == "customDistance") {
                        zoneCameraComp.CustomDistance = std::stof(propValue);
                    }
                    break;
                }
            }
        }
    }
};
