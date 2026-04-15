#pragma once

#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

namespace NFSEngine {
    class ModelComponent : public Component {
    public:
        ModelComponent(GameObject* owner, std::shared_ptr<Model> model, std::shared_ptr<Shader>,
                       std::shared_ptr<Texture> texture = nullptr);

        virtual std::string GetName() const override { return "ModelComponent"; }

    protected:
        virtual void OnAwake() override;
        virtual void OnRender() override;

    private:
        std::shared_ptr<Model> p_Model;
        std::shared_ptr<Shader> p_Shader;
        std::shared_ptr<Texture> p_Texture;

        Transform* p_Transform = nullptr;
    };
} // namespace NFSEngine