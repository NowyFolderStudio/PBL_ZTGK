#pragma once

#include "Components/ModelComponent.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"

using namespace NFSEngine;

class OutlineParameterstLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "OutlineParameters") {

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    auto mat = targetObj->GetComponent<ModelComponent>()->GetMaterial(0);

                    if (propName == "u_OutlineSize") {
                        mat->SetFloat(propName, std::stof(propValue));
                    } else if (propName == "u_OutlineDepthThreshold") {
                        mat->SetFloat(propName, std::stof(propValue));
                    } else if (propName == "u_OutlineNormalThreshold") {
                        mat->SetFloat(propName, std::stof(propValue));
                    } else if (propName == "u_OutlineColor") {
                        mat->SetVec3(propName, ParseVector3(propValue));
                    }
                }
                break;
            }
        }
    }

private:
    glm::vec3 ParseVector3(const std::string& val) const {
        std::string cleanVal = val;

        cleanVal.erase(std::remove(cleanVal.begin(), cleanVal.end(), '('), cleanVal.end());
        cleanVal.erase(std::remove(cleanVal.begin(), cleanVal.end(), ')'), cleanVal.end());
        cleanVal.erase(std::remove(cleanVal.begin(), cleanVal.end(), ' '), cleanVal.end());

        std::stringstream ss(cleanVal);
        std::string item;
        float x = 0.0f, y = 0.0f, z = 0.0f;

        if (std::getline(ss, item, ',')) x = std::stof(item);
        if (std::getline(ss, item, ',')) y = std::stof(item);
        if (std::getline(ss, item, ',')) z = std::stof(item);

        return glm::vec3(x, y, z);
    }
};