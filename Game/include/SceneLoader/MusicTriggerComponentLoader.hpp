#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/MusicTriggerComponent.hpp"

#include <string>

using namespace NFSEngine;

class MusicTriggerComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "MusicTriggerComponent") {

                auto& musicTrigger = targetObj->AddComponent<MusicTriggerComponent>();

                if (comp.contains("properties")) {
                    for (const auto& prop : comp["properties"]) {
                        std::string propName = prop["name"];

                        if (propName == "TargetTrack") {
                            std::string propValue = prop["value"];

                            propValue.erase(0, propValue.find_first_not_of(" \t\r\n"));
                            propValue.erase(propValue.find_last_not_of(" \t\r\n") + 1);

                            if (!propValue.empty()) {
                                musicTrigger.TargetTrack = propValue;
                            }
                        }
                    }
                }

                break;
            }
        }
    }
};