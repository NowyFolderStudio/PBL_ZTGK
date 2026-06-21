#pragma once
#include "Renderer/Material.hpp"
#include "SceneLoader/IComponentLoader.hpp"
#include "Components/ModelComponent.hpp"
#include "Platforms/OpenGL/OpenGLTexture.hpp"
#include <memory>

namespace NFSEngine {
    class ModelComponentLoader : public IComponentLoader {
    private:
        std::shared_ptr<Shader> m_CachedCDShader = nullptr;
        std::shared_ptr<Material> m_CachedCDMaterial = nullptr;

    public:
        void Load(const nlohmann::json& j_obj, GameObject* targetObj, Scene* currentScene) override {
            bool generated = false;
            if (j_obj.contains("mesh_path") && j_obj["mesh_path"] != "") {
                std::shared_ptr<Material> material;
                std::shared_ptr<Shader> shader;
                std::shared_ptr<Model> model;

                material = std::make_shared<NFSEngine::Material>();

                bool isDiffractionCD = false;

                if (j_obj.contains("custom_components")) {
                    for (const auto& comp : j_obj["custom_components"]) {
                        if (comp["name"] == "AudioAnimation") {
                            shader = Shader::Create("AudioShader", "assets/shaders/audioShader.vert",
                                "assets/shaders/toonShaderNew.frag");
                            material->name = "AnimationMaterial";
                            for (const auto& prop : comp["properties"]) {
                                std::string propName = prop["name"];
                                std::string propValue = prop["value"];
                                if (propValue.empty()) continue;
                                if (propName == "u_ScaleStrengthY") {
                                    material->SetFloat("u_ScaleStrengthY", std::stof(propValue));
                                }
                                else if (propName == "u_ScaleStrengthXZ") {
                                    material->SetFloat("u_ScaleStrengthXZ", std::stof(propValue));
                                }
                                else if (propName == "u_BendStrength") {
                                    material->SetFloat("u_BendStrength", std::stof(propValue));
                                }
                                else if (propName == "u_TwistStrength") {
                                    material->SetFloat("u_TwistStrength", std::stof(propValue));
                                }
                            }
                        }
                        else if (comp["name"] == "DiffractionCD") {
                            isDiffractionCD = true;
                        }
                    }
                }

                if (isDiffractionCD) {
                    if (!m_CachedCDShader) {
                        m_CachedCDShader = Shader::Create("CDShader", "assets/shaders/lightShader.vert", "assets/shaders/CDShader.frag");
                    }
                    shader = m_CachedCDShader;

                    if (!m_CachedCDMaterial) {
                        m_CachedCDMaterial = std::make_shared<NFSEngine::Material>();
                        m_CachedCDMaterial->name = "DiffractionMaterial";
                        m_CachedCDMaterial->AlbedoColor = glm::vec3(0.1f, 0.1f, 0.1f);
                        m_CachedCDMaterial->Metallic = 1.0f;
                        m_CachedCDMaterial->Roughness = 0.15f;

                        m_CachedCDMaterial->SetInt("u_UseDiffraction", 1);
                        m_CachedCDMaterial->SetFloat("u_DiffractionDistance", 2000.0f);
                        m_CachedCDMaterial->SetFloat("u_DiffractionStrength", 2.5f);
                    }
                    material = m_CachedCDMaterial;
                }
                else if (!shader) {
                    shader = Shader::Create("BasicShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShaderNew.frag");
                }

                model = std::make_shared<NFSEngine::Model>(j_obj["mesh_path"]);

                if (!isDiffractionCD) {
                    NFSEngine::TextureParameters rampParams;
                    rampParams.WrapS = NFSEngine::TextureWrap::Clamp;
                    rampParams.WrapT = NFSEngine::TextureWrap::Clamp;
                    rampParams.MinFilter = NFSEngine::TextureFilter::Nearest;
                    rampParams.MagFilter = NFSEngine::TextureFilter::Nearest;
                    rampParams.GenerateMipmaps = true;
                    auto map = std::make_shared<NFSEngine::OpenGLTexture>("assets/textures/ramp/RampTexture.png", rampParams);

                    std::string texPath = "assets/textures/cat.png";
                    if (j_obj.contains("texture_path") && j_obj["texture_path"] != "") {
                        texPath = j_obj["texture_path"];
                    }

                    material->AlbedoMap = NFSEngine::Texture::Create(texPath);
                    material->RampMap = map;
                }

                targetObj->AddComponent<ModelComponent>(shader, material);
                targetObj->GetComponent<ModelComponent>()->AddLOD(model, 1000);
            }
        }
    };
}