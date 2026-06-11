#include "Components/ModelComponent.hpp"
#include "Components/AnimatorComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/CullingUtils.hpp"

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
} // namespace NFSEngine