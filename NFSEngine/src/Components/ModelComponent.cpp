#include "Components/ModelComponent.hpp"
#include "Components/AnimatorComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/CullingUtils.hpp"
#include <variant>

namespace NFSEngine {

    ModelComponent::ModelComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> defaultMaterial)
        : Component(owner)
        , m_Shader(std::move(shader)) {

        if (defaultMaterial) {
            m_Materials.push_back(defaultMaterial);
        }
    }

    void ModelComponent::AddLOD(std::shared_ptr<Model> model, float maxDistance) { m_LODs.push_back({ model, maxDistance }); }

    void ModelComponent::OnAwake() {
        m_Transform = m_Owner->GetComponent<Transform>();
        m_Animator = m_Owner->GetComponent<AnimatorComponent>();

        m_LocalBoundingSphere = CullingUtils::GetLocalBoundingSphere(m_Owner);
        m_LocalAABB = CullingUtils::GetLocalAABB(m_Owner);
    }

    void ModelComponent::OnRender() {
        if (m_LODs.empty() || !m_Shader || !m_Transform) return;

        glm::vec3 cameraPos = Renderer::GetCameraPosition();
        float distance = glm::distance(cameraPos, m_Transform->GetPosition());

        std::shared_ptr<Model> selectedModel = nullptr;

        for (const auto& lod : m_LODs) {
            if (distance < lod.MaxDistance) {
                selectedModel = lod.ModelData;
                break;
            }
        }

        if (!selectedModel) return;

        if (Renderer::s_LodEnabled) {
            selectedModel = m_LODs[0].ModelData;
        }

        static const std::vector<glm::mat4> emptyBones;

        const std::vector<glm::mat4>& boneTransforms = m_Animator ? m_Animator->GetFinalBoneMatrices() : emptyBones;

        for (const auto& meshData : selectedModel->GetMeshes()) {

            std::shared_ptr<Material> currentMaterial = nullptr;

            if (meshData.MaterialIndex < m_Materials.size()) {
                currentMaterial = m_Materials[meshData.MaterialIndex];
            } else if (!m_Materials.empty()) {
                currentMaterial = m_Materials[0];
            }

            Renderer::Submit(m_Shader, meshData.VAO, currentMaterial, m_Transform->GetGlobalMatrix(), boneTransforms);
        }
    }

    void ModelComponent::OnImGuiRender() {
        if (ImGui::CollapsingHeader("Model Materials")) {

            for (size_t i = 0; i < m_Materials.size(); i++) {
                auto& mat = m_Materials[i];
                if (!mat) continue;

                std::string headerName = "Material " + std::to_string(i) + (mat->name.empty() ? "" : " (" + mat->name + ")");

                if (ImGui::TreeNode(headerName.c_str())) {
                    std::string idSuffix = "##" + std::to_string(i);

                    ImGui::ColorEdit3(("Albedo Color" + idSuffix).c_str(), &mat->AlbedoColor.x);
                    ImGui::ColorEdit3(("Emissive Color" + idSuffix).c_str(), &mat->EmissiveColor.x);

                    ImGui::DragFloat(("Emissive Strength" + idSuffix).c_str(), &mat->EmissiveStrength, 0.1f, 0.0f, 100.0f);
                    ImGui::SliderFloat(("Metallic" + idSuffix).c_str(), &mat->Metallic, 0.0f, 1.0f);
                    ImGui::SliderFloat(("Roughness" + idSuffix).c_str(), &mat->Roughness, 0.0f, 1.0f);

                    ImGui::Separator();
                    ImGui::Text("Dynamic Properties:");

                    for (auto& [propName, propValue] : mat->Properties) {
                        std::string label = propName + idSuffix;

                        std::visit(
                            [&label](auto& val) {
                                using T = std::decay_t<decltype(val)>;

                                if constexpr (std::is_same_v<T, float>) {
                                    ImGui::DragFloat(label.c_str(), &val, 0.05f);
                                } else if constexpr (std::is_same_v<T, int>) {
                                    ImGui::DragInt(label.c_str(), &val, 1);
                                } else if constexpr (std::is_same_v<T, glm::vec3>) {
                                    ImGui::DragFloat3(label.c_str(), &val.x, 0.05f);
                                } else if constexpr (std::is_same_v<T, glm::vec4>) {
                                    ImGui::DragFloat4(label.c_str(), &val.x, 0.05f);
                                }
                            },
                            propValue);
                    }

                    ImGui::TreePop();
                }
            }
        }
    }
} // namespace NFSEngine