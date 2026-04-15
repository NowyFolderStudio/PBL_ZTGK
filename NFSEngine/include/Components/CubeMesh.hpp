#pragma once

#include <memory>
#include "Components/Transform.hpp"
#include "Components/Component.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/VertexArray.hpp"

namespace NFSEngine {

    class CubeMesh : public Component {
    public:
        CubeMesh(GameObject* owner, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);
        std::string GetName() const override { return "CubeMesh"; }

    protected:
        virtual void OnAwake() override;
        virtual void OnRender() override;

    private:
        std::shared_ptr<Shader> p_Shader;
        std::shared_ptr<Texture> p_Texture;
        std::shared_ptr<VertexArray> p_VertexArray;

        Transform* p_Transform = nullptr;
    };

} // namespace NFSEngine