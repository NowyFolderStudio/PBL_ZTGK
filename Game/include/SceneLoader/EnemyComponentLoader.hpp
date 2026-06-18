#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/Enemy/BasicEnemy.hpp"
#include <string>
#include <sstream>
#include <algorithm>

using namespace NFSEngine;

class EnemyComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "BasicEnemy") {

                auto& enemyComp = targetObj->AddComponent<BasicEnemy>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "PatrolPointA") {
                        enemyComp.PatrolPointA = ParseVec3(propValue);
                    } else if (propName == "PatrolPointB") {
                        enemyComp.PatrolPointB = ParseVec3(propValue);
                    }
                }

                targetObj->AddTag(Tags::Enemy);

                break;
            }
        }
    }

private:
    glm::vec3 ParseVec3(const std::string& valStr) {
        glm::vec3 result(0.0f);

        std::string cleanStr = valStr;
        std::replace(cleanStr.begin(), cleanStr.end(), ',', ' ');

        std::stringstream ss(cleanStr);
        ss >> result.x >> result.y >> result.z;

        return result;
    }
};