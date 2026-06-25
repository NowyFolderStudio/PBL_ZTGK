#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/SpotLight.hpp"
#include "Core/Log.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace NFSEngine;

class SpotLightLoader : public NFSEngine::IComponentLoader {
private:
    glm::vec3 ParseVec3(std::string str) {
        std::string charsToRemove = "RGBA() \t\r\n";
        for (char c : charsToRemove) {
            str.erase(std::remove(str.begin(), str.end(), c), str.end());
        }

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
        return glm::vec3(1.0f);
    }

public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "SpotLight") {

                auto& lightComp = targetObj->AddComponent<SpotLight>();

                if (comp.contains("properties")) {
                    for (const auto& prop : comp["properties"]) {
                        std::string propName = prop["name"];
                        std::string propValue = prop["value"];

                        if (propValue.empty()) continue;

                        if (propName == "Color") {
                            lightComp.Color = ParseVec3(propValue);
                        }
                        else if (propName == "Direction") {
                            lightComp.Direction = ParseVec3(propValue);
                        }
                        else if (propName == "Intensity") {
                            try { lightComp.Intensity = std::stof(propValue); }
                            catch (...) {}
                        }
                        else if (propName == "CutOff") {
                            try { lightComp.CutOff = std::stof(propValue); }
                            catch (...) {}
                        }
                        else if (propName == "OuterCutOff") {
                            try { lightComp.OuterCutOff = std::stof(propValue); }
                            catch (...) {}
                        }
                        else if (propName == "Constant") {
                            try { lightComp.Constant = std::stof(propValue); }
                            catch (...) {}
                        }
                        else if (propName == "Linear") {
                            try { lightComp.Linear = std::stof(propValue); }
                            catch (...) {}
                        }
                        else if (propName == "Quadratic") {
                            try { lightComp.Quadratic = std::stof(propValue); }
                            catch (...) {}
                        }
                    }
                }
                break;
            }
        }
    }
};