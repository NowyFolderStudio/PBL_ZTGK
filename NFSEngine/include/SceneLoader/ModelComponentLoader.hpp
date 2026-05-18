#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ModelComponent.hpp"

namespace NFSEngine {
    class ModelComponentLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            if (j_obj.contains("mesh_path") && j_obj["mesh_path"] != "") {
                auto shader = Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/lightShader.frag");
                auto material = std::make_shared<NFSEngine::Material>();

                std::string texPath = "assets/textures/cat.png"; // Fallback
                if (j_obj.contains("texture_path") && j_obj["texture_path"] != "") {
                    texPath = j_obj["texture_path"];
                }

                material->AlbedoMap = NFSEngine::Texture::Create(texPath);
                targetObj->AddComponent<ModelComponent>(shader, material);

                auto model = std::make_shared<NFSEngine::Model>(j_obj["mesh_path"]);
                targetObj->GetComponent<ModelComponent>()->AddLOD(model, 1000);
            }
        }
    };
} // namespace NFSEngine