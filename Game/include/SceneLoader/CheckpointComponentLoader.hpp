#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/CheckpointComponent.hpp"
#include <algorithm>

using namespace NFSEngine;

class CheckpointComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "CheckpointComponent") {

                auto& checkpointComp = targetObj->AddComponent<CheckpointComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    std::replace(propValue.begin(), propValue.end(), ',', '.');
                    float parsedValue = std::stof(propValue);

                    if (propName == "SpawnOffsetX") {
                        checkpointComp.SpawnOffset.x = parsedValue;
                    } else if (propName == "SpawnOffsetY") {
                        checkpointComp.SpawnOffset.y = parsedValue;
                    } else if (propName == "SpawnOffsetZ") {
                        checkpointComp.SpawnOffset.z = parsedValue;
                    }
                }
                break;
            }
        }
    }
};