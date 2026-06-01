#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/BounceComponent.hpp"

using namespace NFSEngine;

class BounceComponentLoader : public NFSEngine::IComponentLoader {
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

                    if (propName == "BounceHeight") {
                        bounceComp.BounceHeight = std::stof(propValue);
                    }
                }
                break;
            }
        }
    }
};