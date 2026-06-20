#pragma once

#include <memory>
#include <vector>
#include "Components/Component.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"
#include <glm/glm.hpp>

namespace NFSEngine {

    class InstancedModelComponent : public Component {
    public:
        InstancedModelComponent(GameObject* owner, std::shared_ptr<Shader> shader);

        std::string GetName() const override { return "InstancedModelComponent"; }

        void AddLOD(std::shared_ptr<Model> model, float maxDistance);
        void AddInstance(const glm::mat4& transform);
        void SetInstances(const std::vector<glm::mat4>& transforms);
        void ClearInstances();

        void SetMaterial(size_t index, std::shared_ptr<Material> material);

    protected:
        virtual void OnRender() override;

    private:
        struct LODLevel {
            std::shared_ptr<Model> ModelData;
            float MaxDistance;
        };

        std::vector<LODLevel> m_LODs;
        std::shared_ptr<Shader> m_Shader;
        std::vector<std::shared_ptr<Material>> m_Materials;

        std::vector<glm::mat4> m_Transforms;
    };

} // namespace NFSEngine