#pragma once

#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/StartingPoint.hpp" // Upewnij się, że to ścieżka do Twojego pliku

using namespace NFSEngine;

class StartingPointLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {
            if (comp["name"] == "StartingPoint") {
                targetObj->AddComponent<StartingPoint>();
                break;
            }
        }
    }
};