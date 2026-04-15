#include "Components/ModelComponent.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine {
    ModelComponent::ModelComponent(GameObject* owner, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader,
                                   std::shared_ptr<Texture> texture)
        : Component(owner)
        , p_Model(std::move(model))
        , p_Shader(std::move(shader))
        , p_Texture(std::move(texture)) { }

    void ModelComponent::OnAwake() { p_Transform = m_Owner->GetComponent<Transform>(); }

    void ModelComponent::OnRender() {
        if (!p_Model || !p_Shader || !p_Transform) return;

        const auto& meshes = p_Model->GetMeshes();

        for (const auto& meshVAO : meshes) {
            Renderer::Submit(p_Shader, meshVAO, p_Texture, p_Transform->GetGlobalMatrix());
        }
    }
} // namespace NFSEngine