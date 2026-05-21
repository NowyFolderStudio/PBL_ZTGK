#pragma once
#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ModelComponent.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"

namespace NFSEngine {
    class ModelComponentLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            if (j_obj.contains("mesh_path") && j_obj["mesh_path"] != "") {
                auto shader = Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShader.frag");
                NFSEngine::TextureParameters rampParams;
                rampParams.WrapS = NFSEngine::TextureWrap::Clamp;
                rampParams.WrapT = NFSEngine::TextureWrap::Clamp;
                rampParams.MinFilter = NFSEngine::TextureFilter::Nearest;
                rampParams.MagFilter = NFSEngine::TextureFilter::Nearest;
                auto material = std::make_shared<NFSEngine::Material>();
                auto ramp = std::make_shared<OpenGLTexture>("assets/textures/ramp/RampTexture.png", rampParams);

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