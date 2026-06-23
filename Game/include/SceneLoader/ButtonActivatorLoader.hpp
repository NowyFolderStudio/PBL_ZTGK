#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ButtonActivator.hpp"
#include <string>

class ButtonActivatorLoader : public NFSEngine::IComponentLoader {
public:
    void Load(const nlohmann::json& j_obj, NFSEngine::GameObject* targetObj, NFSEngine::Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) return;

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "ButtonActivator") {
                targetObj->AddComponent<ButtonActivator>();
                break;
            }
        }
    }

    void PostLoad(const nlohmann::json& j_obj, NFSEngine::GameObject* targetObj,
                  const std::unordered_map<int, NFSEngine::GameObject*>& instanceMap) override {
        if (!j_obj.contains("custom_components")) return;

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "ButtonActivator") {
                auto* activatorComp = targetObj->GetComponent<ButtonActivator>();
                if (!activatorComp) continue;

                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];

                    if (propValue.empty()) continue;

                    if (propName == "ActivatedObject") {
                        int targetID = std::stoi(propValue);

                        auto it = instanceMap.find(targetID);
                        if (it != instanceMap.end()) {
                            activatorComp->SetActivatedObject(it->second);
                        } else {
                            NFS_CORE_WARN("ButtonActivator cant find GameObject with ID: {0}", targetID);
                        }
                    }
                }
                break;
            }
        }
    }
};