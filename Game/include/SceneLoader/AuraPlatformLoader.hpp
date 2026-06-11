#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/AuraPlatform.hpp"
#include "Components/Aura/AuraManager.hpp"

using namespace NFSEngine;

class AuraPlatformLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "AuraPlatform") {

                auto& auraPlatformComp = targetObj->AddComponent<AuraPlatform>();
                targetObj->AddComponent<ScaleAnimator>();
                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "RequiredAura") {
                        auraPlatformComp.RequiredAura = ParseAuraType(propValue);
                    }
                }
                break;
            }
        }
    }

    AuraType ParseAuraType(const std::string& auraStr) {
        if (auraStr == "Piano") {
            return AuraType::First;
        } else if (auraStr == "Bass") {
            return AuraType::Second;
        }
        return AuraType::First;
    }
};