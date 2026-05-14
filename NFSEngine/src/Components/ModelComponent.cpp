#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine {

    ModelComponent::ModelComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material)
        : Component(owner)
        , m_Shader(std::move(shader))
        , m_Material(std::move(material)) { }

    void ModelComponent::AddLOD(std::shared_ptr<Model> model, float maxDistance) {
        m_LODs.push_back({ model, maxDistance });
    }

    void ModelComponent::OnAwake() { m_Transform = m_Owner->GetComponent<Transform>(); }

    void ModelComponent::OnRender() {
        if (m_LODs.empty() || !m_Shader || !m_Transform) return;

        glm::vec3 cameraPos = Renderer::GetCameraPosition();
        float distance = glm::distance(cameraPos, m_Transform->GetPosition());

        std::shared_ptr<Model> selectedModel = nullptr;;

        for (const auto& lod : m_LODs) {
            if (distance < lod.MaxDistance) {
                selectedModel = lod.ModelData;
                break;
            }
        }

        if (!selectedModel) return;

        for (auto& vao : selectedModel->GetMeshes()) {
            Renderer::Submit(m_Shader, vao, m_Material, m_Transform->GetGlobalMatrix());
        }
    }
}