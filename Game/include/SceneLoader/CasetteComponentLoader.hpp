#pragma once

#include "Core/Log.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/CasetteComponent.hpp"

#include <sstream>
#include <string>

using namespace NFSEngine;

class CasetteComponentLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "CasetteComponent") {

                auto& casetteComp = targetObj->AddComponent<CasetteComponent>();

                if (comp.contains("properties")) {
                    for (const auto& prop : comp["properties"]) {
                        std::string propName = prop["name"];

                        if (propName == "TracksToUnlock") {
                            std::string propValue = prop["value"];

                            std::stringstream ss(propValue);
                            std::string trackName;

                            while (std::getline(ss, trackName, ',')) {
                                trackName.erase(0, trackName.find_first_not_of(" \t"));
                                trackName.erase(trackName.find_last_not_of(" \t") + 1);

                                if (!trackName.empty()) {
                                    casetteComp.TracksToUnlock.push_back(trackName);
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
    }
};