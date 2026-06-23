#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/InteractivePiano.hpp"
#include <string>

using namespace NFSEngine;

class InteractivePianoLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "InteractivePiano") {

                auto& pianoComp = targetObj->AddComponent<InteractivePiano>();

                if (comp.contains("properties")) {
                    for (const auto& prop : comp["properties"]) {
                        std::string propName = prop["name"];
                        std::string propValue = prop["value"];

                        if (propValue.empty()) continue;

                        if (propName == "AudioFile") {
                            std::string fullPath = "assets/audio/sounds/" + propValue;
                            pianoComp.LoadPiano(fullPath);
                        }
                    }
                }
                break;
            }
        }
    }
};