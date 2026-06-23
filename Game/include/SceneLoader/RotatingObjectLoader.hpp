#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
// Pamiętaj, aby upewnić się, że ścieżka do pliku jest poprawna
#include "Components/RotatingObject.hpp"

#include <sstream>
#include <string>
#include <glm/vec3.hpp>

using namespace NFSEngine;

class RotatingObjectLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        // Sprawdzamy, czy obiekt posiada niestandardowe komponenty
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "RotatingObject") {

                auto& rotatingObj = targetObj->AddComponent<RotatingObject>();

                glm::vec3 rotationDirection(0.0f);
                float rotationSpeed = 0.0f;

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "RotationSpeed") {
                        rotationSpeed = std::stof(propValue);
                    } else if (propName == "RotationDirection") {
                        std::stringstream ss(propValue);
                        std::string token;
                        float values[3] = { 0.0f, 0.0f, 0.0f };
                        int i = 0;

                        while (std::getline(ss, token, ',') && i < 3) {
                            values[i++] = std::stof(token);
                        }

                        rotationDirection = glm::vec3(values[0], -values[1], -values[2]);
                    }
                }

                // Aplikujemy zebrane dane do komponentu
                rotatingObj.SetRotation(rotationDirection, rotationSpeed);
                break; // Znaleźliśmy nasz komponent, możemy opuścić pętlę
            }
        }
    }
};