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

            if (comp["name"] == "EnemyComponent") {

                auto& enemyComp = targetObj->AddComponent<BasicEnemy>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];

                    if (prop.contains("vector3_value") && !prop["vector3_value"].is_null()) {

                        auto propValue = prop["vector3_value"];
                        if (propValue.size() == 3) {
                            glm::vec3 parsedVec(propValue[0].get<float>(), propValue[1].get<float>(), propValue[2].get<float>());

                            if (propName == "PatrolPointA") {
                                enemyComp.PatrolPointA = parsedVec;
                            } else if (propName == "PatrolPointB") {
                                enemyComp.PatrolPointB = parsedVec;
                            }
                        }
                    }
                }

                if (!targetObj->HasComponent<RigidBody3DComponent>()) {
                    targetObj->AddComponent<RigidBody3DComponent>();
                }
                targetObj->AddTag(Tags::Enemy);

                break;
            }
        }
    }
};