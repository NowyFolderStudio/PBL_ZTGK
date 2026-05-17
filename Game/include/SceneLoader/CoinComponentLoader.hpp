#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"
#include "Components/CoinComponent.hpp"

using namespace NFSEngine;

class CoinComponentLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "Coin") {

                auto& coinComp = targetObj->AddComponent<CoinComponent>();
                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "value") {
                        coinComp.ScoreValue = std::stoi(propValue);
                    }
                }
                break;
            }
        }
    }
};