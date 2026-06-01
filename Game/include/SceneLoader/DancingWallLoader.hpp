#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/DancingWall.hpp"

#include <string>

using namespace NFSEngine;

class DancingWallLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "DancingWall") {

                auto& wallComp = targetObj->AddComponent<DancingWall>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "TargetTrack") {
                        wallComp.TargetTrack = propValue;
                    }
                    else if (propName == "PopOutSpeed") {
                        wallComp.PopOutSpeed = std::stof(propValue);
                    }
                    else if (propName == "ReturnSpeed") {
                        wallComp.ReturnSpeed = std::stof(propValue);
                    }
                }

                break;
            }
        }
    }
};