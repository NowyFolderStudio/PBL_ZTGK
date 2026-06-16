#pragma once
#include "Renderer/Material.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ModelComponent.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"
#include <memory>

namespace NFSEngine {
    class ModelComponentLoader : public IComponentLoader {
    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            bool generated = false;
            if (j_obj.contains("mesh_path") && j_obj["mesh_path"] != "") {
                std::shared_ptr<Material> material;
                std::shared_ptr<Shader> shader;
                std::shared_ptr<Model> model;
                if (j_obj.contains("custom_components")) {
                    for (const auto& comp : j_obj["custom_components"]) {
                        if (comp["name"] == "GeneratywnyKomponent") {
                            shader = Shader::Create("AudioShader", "assets/shaders/audioShader.vert",
                                                    "assets/shaders/toonShader.frag");
                            material = std::make_shared<Material>();
                            material->name = "AnimationMaterial";
                            for (const auto& prop : comp["properties"]) {
                                std::string propName = prop["name"];
                                std::string propValue = prop["value"];
                                if (propValue.empty()) continue;
                                if (propName == "u_ScaleStrengthY") {
                                    material->SetFloat("u_ScaleStrengthY", std::stof(propValue));
                                } else if (propName == "u_ScaleStrengthXZ") {
                                    material->SetFloat("u_ScaleStrengthXZ", std::stof(propValue));
                                } else if (propName == "u_BendStrength") {
                                    material->SetFloat("u_BendStrength", std::stof(propValue));
                                } else if (propName == "u_TwistStrength") {
                                    material->SetFloat("u_TwistStrength", std::stof(propValue));
                                }
                                model = std::make_shared<NFSEngine::Model>("assets/models/glosnik/glosnik_wieza.fbx");
                                targetObj->GetTransform()->SetScale({ 1, 1, 1 });
                            }
                            generated = true;
                            break;
                        }
                    }
                }
                if (!generated) {
                    shader = Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
                    material = std::make_shared<NFSEngine::Material>();
                    model = std::make_shared<NFSEngine::Model>(j_obj["mesh_path"]);
                }
                NFSEngine::TextureParameters rampParams;
                rampParams.WrapS = NFSEngine::TextureWrap::Clamp;
                rampParams.WrapT = NFSEngine::TextureWrap::Clamp;
                rampParams.MinFilter = NFSEngine::TextureFilter::Nearest;
                rampParams.MagFilter = NFSEngine::TextureFilter::Nearest;
                rampParams.GenerateMipmaps = false;
                auto map = std::make_shared<NFSEngine::OpenGLTexture>("assets/textures/ramp/RampTexture.png", rampParams);

                std::string texPath = "assets/textures/cat.png"; // Fallback
                if (j_obj.contains("texture_path") && j_obj["texture_path"] != "") {
                    texPath = j_obj["texture_path"];
                }

                material->AlbedoMap = NFSEngine::Texture::Create(texPath);
                material->RampMap = map;
                targetObj->AddComponent<ModelComponent>(shader, material);
                targetObj->GetComponent<ModelComponent>()->AddLOD(model, 1000);
            }
        }
    };
} // namespace NFSEngine