#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/DialogueTriggerComponent.hpp"
#include <algorithm>

using namespace NFSEngine;

class DialogueComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "DialogueComponent") {

                auto& dialogueComp = targetObj->AddComponent<DialogueTriggerComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "Message") {
                        dialogueComp.Message = propValue;
                    } else if (propName == "Duration") {
                        std::replace(propValue.begin(), propValue.end(), ',', '.');
                        dialogueComp.Duration = std::stof(propValue);
                    }
                    break;
                }
            }
        }
    }
};