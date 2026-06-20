#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/BounceComponent.hpp"

using namespace NFSEngine;

class BounceComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "BounceComponent") {

                auto& bounceComp = targetObj->AddComponent<BounceComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "BaseBounceHeight") {
                        bounceComp.BaseBounceHeight = std::stof(propValue);
                    } else if (propName == "BounceMultiplier") {
                        bounceComp.BounceMultiplier = std::stof(propValue);
                    }
                }
                break;
            }
        }
    }
};