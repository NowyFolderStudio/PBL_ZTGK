#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/TutorialTriggerComponent.hpp"
#include "Components/Managers/TutorialManager.hpp"
#include <string>

using namespace NFSEngine;

class TutorialTriggerLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "TutorialTrigger") {

                auto& triggerComp = targetObj->AddComponent<TutorialTriggerComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "Panel") {
                        NFS_INFO("Setting TutorialTrigger panel to: {0}", propValue);
                        triggerComp.PanelToDisplay = StringToTutorialPanel(propValue);
                    }
                }

                break;
            }
        }
    }

private:
    TutorialPanel StringToTutorialPanel(const std::string& value) {
        if (value == "Walk") return TutorialPanel::Walk;
        if (value == "LookAround") return TutorialPanel::LookAround;
        if (value == "Jump") return TutorialPanel::Jump;
        if (value == "Pause") return TutorialPanel::Pause;
        if (value == "Checkpoint") return TutorialPanel::Checkpoint;
        if (value == "WallJump") return TutorialPanel::WallJump;
        if (value == "Aura") return TutorialPanel::Aura;
        if (value == "DoubleJump") return TutorialPanel::DoubleJump;
        if (value == "CDPlayer") return TutorialPanel::CDPlayer;
        if (value == "Attack") return TutorialPanel::Attack;

        NFS_WARN("Unknown TutorialPanel value: {0}. Defaulting to Walk.", value);
        return TutorialPanel::Walk;
    }
};