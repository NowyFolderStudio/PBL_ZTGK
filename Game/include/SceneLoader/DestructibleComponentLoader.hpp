#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/DestructibleComponent.hpp"
#include <string>

using namespace NFSEngine;

class DestructibleComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "DestructibleComponent") {

                auto& destComp = targetObj->AddComponent<DestructibleComponent>();

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "MaxHealth") {
                        destComp.MaxHealth = std::stoi(propValue);
                    } else if (propName == "SpawnParticlesOnDeath") {
                        destComp.SpawnParticlesOnDeath = (propValue == "True" || propValue == "true");
                    }
                    break;
                }

                if (!targetObj->HasComponent<RigidBody3DComponent>()) {
                    targetObj->AddComponent<RigidBody3DComponent>();
                }
            }
        }
    }
};