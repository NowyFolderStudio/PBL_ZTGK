#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/Doors.hpp"
#include <string>
#include <sstream>
#include <vector>

using namespace NFSEngine;

class DoorLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "Door") {

                auto& doorsComp = targetObj->AddComponent<Doors>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];

                    if (propName == "OpenVectorRotation") {
                        std::string propValue = prop["value"];

                        if (!propValue.empty()) {
                            // Rozbijamy string "X,Y,Z" po przecinkach
                            std::stringstream ss(propValue);
                            std::string item;
                            std::vector<float> values;

                            while (std::getline(ss, item, ',')) {
                                values.push_back(std::stof(item)); // std::stof zamienia string na float
                            }

                            // Jeśli udało się odczytać 3 wartości, przypisujemy
                            if (values.size() >= 3) {
                                doorsComp.OpenVectorRotation = glm::vec3(values[0], values[1], values[2]);
                            }
                        }
                    } else if (propName == "AnimationSpeed") {
                        std::string propValue = prop["value"];
                        if (!propValue.empty()) {
                            doorsComp.AnimationSpeed = std::stof(propValue);
                        }
                    }
                }

                break;
            }
        }
    }
};