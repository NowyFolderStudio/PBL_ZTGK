#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/Enemy/BasicEnemy.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Components/DestructibleComponent.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

using namespace NFSEngine;

class EnemyComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "EnemyComponent") {

                targetObj->GetTransform()->SetScale({ 2.0f, 2.0f, 2.0f });

                if (!targetObj->HasComponent<ModelComponent>()) {
                    auto material = std::make_shared<Material>();
                    auto shader = Shader::Create("EnemyGlitchShader", "assets/shaders/enemy_glitch.vert",
                                                 "assets/shaders/enemy_glitch.frag");
                    auto model = std::make_shared<Model>("assets/models/lowsphere/scene.gltf");

                    targetObj->AddComponent<ModelComponent>(shader, material);
                    targetObj->GetComponent<ModelComponent>()->AddLOD(model, 10000.0f);
                }

                if (!targetObj->HasComponent<RigidBody3DComponent>()) {
                    targetObj->AddComponent<RigidBody3DComponent>();
                }
                if (!targetObj->HasComponent<CapsuleCollider3DComponent>()) {
                    auto& boxCollider = targetObj->AddComponent<CapsuleCollider3DComponent>();
                    boxCollider.Offset = glm::vec3(0.0f, -0.5f, 0.0f);
                }

                targetObj->AddTag(Tags::Enemy);
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
                break;
            }
        }
    }
};