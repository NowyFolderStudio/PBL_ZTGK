#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ConsolePuzzle.hpp"
#include <sstream>
#include <string>
#include <cstdio>

namespace NFSEngine {

    class ConsolePuzzleLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            if (!j_obj.contains("custom_components")) return;

            for (const auto& comp : j_obj["custom_components"]) {
                if (comp["name"] == "ConsolePuzzle") {

                    targetObj->AddComponent<ConsolePuzzle>();
                    auto* puzzle = targetObj->GetComponent<ConsolePuzzle>();

                    if (comp.contains("properties")) {
                        for (const auto& prop : comp["properties"]) {
                            std::string propName = prop["name"];
                            std::string propValue = prop["value"].get<std::string>();

                            if (propName == "ButtonOrder") {
                                std::stringstream ss(propValue);
                                int val;
                                while (ss >> val) {
                                    puzzle->buttonOrder.push_back(val);
                                }
                            } else if (propName == "WrongColor") {
                                puzzle->wrongColor = ParseColor(propValue);
                            } else if (propName == "CorrectColor") {
                                puzzle->correctColor = ParseColor(propValue);
                            } else if (propName == "SolutionColor") {
                                puzzle->solutionColor = ParseColor(propValue);
                            } else if (propName == "SolutionSpeed") {
                                try {
                                    puzzle->solutionSpeed = std::stof(propValue);
                                } catch (...) {
                                    puzzle->solutionSpeed = 1.0f;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }

        void PostLoad(const nlohmann::json& j_obj, GameObject* targetObj,
                      const std::unordered_map<int, GameObject*>& instanceMap) override {
            if (!j_obj.contains("custom_components")) return;

            for (const auto& comp : j_obj["custom_components"]) {
                if (comp["name"] == "ConsolePuzzle") {

                    auto* puzzle = targetObj->GetComponent<ConsolePuzzle>();
                    if (!puzzle) return;

                    if (comp.contains("properties")) {
                        for (const auto& prop : comp["properties"]) {
                            if (prop["name"] == "Buttons") {
                                std::string idStr = prop["value"].get<std::string>();
                                if (!idStr.empty()) {
                                    int buttonsId = std::stoi(idStr);

                                    auto it = instanceMap.find(buttonsId);
                                    if (it != instanceMap.end()) {
                                        puzzle->buttonsContainer = it->second;
                                    } else {
                                        NFS_CORE_WARN("ConsolePuzzleLoader: Nie znaleziono obiektu Buttons o ID {0}", buttonsId);
                                    }
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

    private:
        glm::vec3 ParseColor(const std::string& colorStr) {
            float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
            if (sscanf(colorStr.c_str(), "RGBA(%f, %f, %f, %f)", &r, &g, &b, &a) == 4) {
                return glm::vec3(r, g, b);
            }
            return glm::vec3(1.0f);
        }
    };

} // namespace NFSEngine