#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/RhythmPlatform.hpp" // Upewnij siê, ¿e œcie¿ka jest poprawna

#include <string>

using namespace NFSEngine;

class RhythmPlatformLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "RhythmPlatform") {

                auto& platformComp = targetObj->AddComponent<RhythmPlatform>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "TargetTrack") {
                        platformComp.TargetTrack = propValue;
                    }
                    else if (propName == "StartsActive") {
                        if (propValue == "True" || propValue == "true" || propValue == "1") {
                            platformComp.StartsActive = true;
                        }
                        else {
                            platformComp.StartsActive = false;
                        }
                    }
                }

                break;
            }
        }
    }
};