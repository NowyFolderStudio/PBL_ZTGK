#pragma once

#include <vector>

#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"


namespace NFSEngine {

    struct LODLevel {
        std::shared_ptr<Model> ModelData;
        float MaxDistance;
    };

    class ModelComponent : public Component {
    public:
        ModelComponent(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material = nullptr);

        virtual std::string GetName() const override { return "ModelComponent"; }

        void AddLOD(std::shared_ptr<Model> model, float maxDistance);

        const std::vector<LODLevel>& GetLODs() const { return m_LODs; }

    protected:
        virtual void OnAwake() override;
        virtual void OnRender() override;

    private:
        std::vector<LODLevel> m_LODs;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Material> m_Material;

        Transform* m_Transform = nullptr;
    };
} // namespace NFSEngine