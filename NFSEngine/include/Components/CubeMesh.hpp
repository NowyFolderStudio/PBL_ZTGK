#pragma once

#include <memory>
#include "Components/Transform.hpp"
#include "Components/Component.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Frustum.hpp"

namespace NFSEngine {

    class CubeMesh : public Component {
    public:
        CubeMesh(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material);
        std::string GetName() const override { return "CubeMesh"; }
        std::shared_ptr<Material> GetMaterial() const { return p_Material; }
        BoundingSphere GetCachedLocalSphere() const { return m_LocalBoundingSphere; }
        std::pair<glm::vec3, glm::vec3> GetCachedLocalAABB() const { return m_LocalAABB; }

    protected:
        virtual void OnAwake() override;
        virtual void OnRender() override;

    private:
        std::shared_ptr<Shader> p_Shader;
        std::shared_ptr<Material> p_Material;
        std::shared_ptr<VertexArray> p_VertexArray;

        BoundingSphere m_LocalBoundingSphere;
        std::pair<glm::vec3, glm::vec3> m_LocalAABB;

        Transform* p_Transform = nullptr;
    };

} // namespace NFSEngine