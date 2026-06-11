#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/ConsoleButtonComponent.hpp"
#include <string>
#include <algorithm>

using namespace NFSEngine;

class ConsoleButtonLoader : public NFSEngine::IComponentLoader {
private:
    glm::vec3 ParseVec3(const std::string& val) {
        glm::vec3 vec(1.0f);
        std::string cleanVal = val;

        std::string charsToRemove = "RGBArgba() ";
        for (char c : charsToRemove) {
            cleanVal.erase(std::remove(cleanVal.begin(), cleanVal.end(), c), cleanVal.end());
        }

        size_t pos1 = cleanVal.find(',');
        size_t pos2 = cleanVal.find(',', pos1 + 1);

        try {
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                vec.x = std::stof(cleanVal.substr(0, pos1));
                vec.y = std::stof(cleanVal.substr(pos1 + 1, pos2 - pos1 - 1));
                vec.z = std::stof(cleanVal.substr(pos2 + 1));
            }
        }
        catch (...) {
        }

        return vec;
    }

public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) return;

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "ConsoleButton") {

                auto& buttonComp = targetObj->AddComponent<ConsoleButtonComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "BaseColor") {
                        buttonComp.BaseColor = ParseVec3(propValue);
                    }
                    else if (propName == "ActiveColor") {
                        buttonComp.ActiveColor = ParseVec3(propValue);
                    }
                    else if (propName == "BaseStrength") {
                        buttonComp.BaseStrength = std::stof(propValue);
                    }
                    else if (propName == "ActiveStrength") {
                        buttonComp.ActiveStrength = std::stof(propValue);
                    }
                    else if (propName == "SoundFileName") {
                        buttonComp.SoundFilePath = "assets/audio/sounds/" + propValue;
                    }
                }
                break;
            }
        }
    }
};