#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/CDBoxComponent.hpp"

#include <string>
#include <sstream>
#include <vector>

using namespace NFSEngine;

class CDBoxComponentLoader : public NFSEngine::IComponentLoader {
private:
    glm::vec3 ParseVec3(const std::string& str) {
        std::stringstream ss(str);
        std::string token;
        std::vector<float> values;

        while (std::getline(ss, token, ',')) {
            try {
                values.push_back(std::stof(token));
            }
            catch (...) {
                values.push_back(0.0f);
            }
        }

        if (values.size() >= 3) {
            return glm::vec3(values[0], values[1], values[2]);
        }
        return glm::vec3(0.0f);
    }

public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "CDBoxComponent") {

                auto& cdBoxComp = targetObj->AddComponent<CDBoxComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "EjectOffset") {
                        cdBoxComp.EjectOffset = ParseVec3(propValue);
                    }
                    else if (propName == "EjectDuration") {
                        try {
                            cdBoxComp.EjectDuration = std::stof(propValue);
                        }
                        catch (...) {
                            cdBoxComp.EjectDuration = 0.5f;
                        }
                    }
                    else if (propName == "ButtonPressOffset") {
                        cdBoxComp.ButtonPressOffset = ParseVec3(propValue);
                    }
                }

                break;
            }
        }
    }
};