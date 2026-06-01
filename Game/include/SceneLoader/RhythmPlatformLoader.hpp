#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/RhythmPlatform.hpp"

using namespace NFSEngine;

class RhythmPlatformLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "RhythmPlatform") {

                auto& rhythmPlatformComp = targetObj->AddComponent<RhythmPlatform>();
                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "TargetTrack") {
                        rhythmPlatformComp.TargetTrack = propValue;
                    }

                    if (propName == "StartsActive") {
                        rhythmPlatformComp.StartsActive = (propValue == "true");
                    }

                    break;
                }
            }
        }
    };