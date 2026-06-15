#include "Components/InstancedModelComponent.hpp"
#include "Renderer/Renderer.hpp"

namespace NFSEngine {

    InstancedModelComponent::InstancedModelComponent(GameObject* owner, std::shared_ptr<Shader> shader)
        : Component(owner)
        , m_Shader(std::move(shader)) { }

    void InstancedModelComponent::AddLOD(std::shared_ptr<Model> model, float maxDistance) {
        m_LODs.push_back({ model, maxDistance });
    }

    void InstancedModelComponent::AddInstance(const glm::mat4& transform) { m_Transforms.push_back(transform); }

    void InstancedModelComponent::SetInstances(const std::vector<glm::mat4>& transforms) { m_Transforms = transforms; }

    void InstancedModelComponent::ClearInstances() { m_Transforms.clear(); }

    void InstancedModelComponent::SetMaterial(size_t index, std::shared_ptr<Material> material) {
        if (index >= m_Materials.size()) m_Materials.resize(index + 1);
        m_Materials[index] = material;
    }

    void InstancedModelComponent::OnRender() {
        if (m_Transforms.empty() || m_LODs.empty() || !m_Shader) return;

        glm::vec3 cameraPos = Renderer::GetCameraPosition();

        std::vector<std::vector<glm::mat4>> lodBuckets(m_LODs.size());

        for (const auto& transform : m_Transforms) {
            glm::vec3 position = glm::vec3(transform[3]);
            float distance = glm::distance(cameraPos, position);

            for (size_t i = 0; i < m_LODs.size(); ++i) {
                if (distance < m_LODs[i].MaxDistance) {
                    lodBuckets[i].push_back(transform);
                    break;
                }
            }
        }

        for (size_t i = 0; i < m_LODs.size(); ++i) {
            if (lodBuckets[i].empty()) continue;

            std::shared_ptr<Model> model = m_LODs[i].ModelData;

            for (const auto& meshData : model->GetMeshes()) {
                std::shared_ptr<Material> currentMaterial = nullptr;
                if (meshData.MaterialIndex < m_Materials.size()) {
                    currentMaterial = m_Materials[meshData.MaterialIndex];
                } else if (!m_Materials.empty()) {
                    currentMaterial = m_Materials[0];
                }

                Renderer::SubmitInstanced(m_Shader, meshData.VAO, currentMaterial, lodBuckets[i]);
            }
        }
    }

} // namespace NFSEngine