#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/PortalComponent.hpp"
#include "Components/CubeMesh.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Shader.hpp"
#include "GameManager.hpp"
#include <string>
#include <memory>

using namespace NFSEngine;

class PortalComponentLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "PortalComponent") {

                auto portalShader = Shader::Create("PortalShader", "assets/shaders/portal.vert", "assets/shaders/portal.frag");
                auto portalMaterial = std::make_shared<Material>();

                if (!targetObj->HasComponent<CubeMesh>()) {
                    targetObj->AddComponent<CubeMesh>(portalShader, portalMaterial);
                }

                if (!targetObj->HasComponent<BoxCollider3DComponent>()) {
                    auto& boxCollider = targetObj->AddComponent<BoxCollider3DComponent>();
                    boxCollider.IsTrigger = true;
                }

                auto& portalComp = targetObj->AddComponent<PortalComponent>();
                portalComp.PortalMaterial = portalMaterial;

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "TargetName") {
                        NFS_INFO("Setting portal target name to: {0}", propValue);
                        portalComp.TargetName = propValue;
                    }
                }

                portalComp.OnTriggerCallback = [targetLevel = portalComp.TargetName]() {
                    NFS_INFO("Portal activated! Target: {0}", targetLevel);

                    if (targetLevel == "Outro") {
                        GameManager::Get().RequestStateChange(GameState::Outro);
                    } else {
                        GameManager::Get().LoadLevel(targetLevel);
                    }
                };
                break;
            }
        }
    }
};