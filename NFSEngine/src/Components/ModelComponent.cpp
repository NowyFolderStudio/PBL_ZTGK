#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine {

    ModelComponent::ModelComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> defaultMaterial)
        : Component(owner)
        , m_Shader(std::move(shader)) {

        if (defaultMaterial) {
            m_Materials.push_back(defaultMaterial);
        }
    }

    void ModelComponent::AddLOD(std::shared_ptr<Model> model, float maxDistance) {
        m_LODs.push_back({ model, maxDistance });
    }

    void ModelComponent::OnAwake() { m_Transform = m_Owner->GetComponent<Transform>(); }

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

        for (const auto& meshData : selectedModel->GetMeshes()) {

            std::shared_ptr<Material> currentMaterial = nullptr;

            if (meshData.MaterialIndex < m_Materials.size()) {
                currentMaterial = m_Materials[meshData.MaterialIndex];
            }
            else if (!m_Materials.empty()) {
                currentMaterial = m_Materials[0];
            }

            Renderer::Submit(m_Shader, meshData.VAO, currentMaterial, m_Transform->GetGlobalMatrix());
        }
    }
}