#pragma once

#include <memory>
#include <vector>

#include "Components/AnimatorComponent.hpp"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Frustum.hpp"

namespace NFSEngine {

    struct LODLevel {
        std::shared_ptr<Model> ModelData;
        float MaxDistance;
    };

    class ModelComponent : public Component {
    public:
        ModelComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> defaultMaterial = nullptr);

        virtual std::string GetName() const override { return "ModelComponent"; }

        virtual void OnImGuiRender() override;

        void AddLOD(std::shared_ptr<Model> model, float maxDistance);

        const std::vector<LODLevel>& GetLODs() const { return m_LODs; }

        void SetMaterial(size_t index, std::shared_ptr<Material> material) {
            if (index >= m_Materials.size()) {
                m_Materials.resize(index + 1);
            }
            m_Materials[index] = material;
        }

        std::shared_ptr<Material> GetMaterial(int index) { return m_Materials[index]; }
        std::vector<std::shared_ptr<Material>> GetMaterials() { return m_Materials; }
        BoundingSphere GetCachedLocalSphere() const { return m_LocalBoundingSphere; }
        std::pair<glm::vec3, glm::vec3> GetCachedLocalAABB() const { return m_LocalAABB; }

    protected:
        virtual void OnAwake() override;
        virtual void OnRender() override;

    private:
        std::vector<LODLevel> m_LODs;
        std::shared_ptr<Shader> m_Shader;
        std::vector<std::shared_ptr<Material>> m_Materials;

        BoundingSphere m_LocalBoundingSphere;
        std::pair<glm::vec3, glm::vec3> m_LocalAABB;

        Transform* m_Transform = nullptr;
        AnimatorComponent* m_Animator = nullptr;
    };
} // namespace NFSEngine