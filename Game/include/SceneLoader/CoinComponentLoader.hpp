#pragma once

#include "Components/ModelComponent.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include "Core/Scene.hpp"
#include "Core/GameObject.hpp"
#include "Components/CoinComponent.hpp"
#include <memory>

using namespace NFSEngine;

class CoinComponentLoader : public NFSEngine::IComponentLoader {
    void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
        if (!j_obj.contains("custom_components")) {
            return;
        }

        for (const auto& comp : j_obj["custom_components"]) {

            if (comp["name"] == "Coin") {

                auto& coinComp = targetObj->AddComponent<CoinComponent>();
                for (const auto& prop : comp["properties"]) {
                    std::string propName = prop["name"];
                    std::string propValue = prop["value"];
                    if (propValue.empty()) continue;

                    if (propName == "value") {
                        coinComp.ScoreValue = std::stoi(propValue);
                    }
                }
                auto material = std::make_shared<Material>();
                TextureParameters texPar;
                texPar.GenerateMipmaps = false;
                auto texture = Texture::Create("assets/models/Notes/note_yellow.png", texPar);
                material->AlbedoMap = texture;
                material->SetFloat("u_OutlineDepthThreshold", 0.2);
                material->SetFloat("u_OutlineNormalThreshold", 0.5);
                auto shader = Shader::Create("NoteShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShader.frag");
                auto* modelObject = currentScene->CreateGameObject("ModelObject");
                modelObject->AddComponent<ModelComponent>(shader, material);
                auto model = std::make_shared<Model>("assets/models/Notes/nuta2.fbx");
                modelObject->GetComponent<ModelComponent>()->AddLOD(model, 1000);
                modelObject->GetTransform()->SetParent(targetObj->GetTransform());
                modelObject->GetTransform()->Rotate({ -90, 0, 0 });
                modelObject->GetTransform()->SetPosition({ 0, 0, 0 });
                modelObject->GetTransform()->SetScale({ 0.25, 0.25, 0.25 });
                break;
            }
        }
    }
};