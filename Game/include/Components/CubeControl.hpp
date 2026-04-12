#include "Components/Transform.hpp"
#include "Core/KeyCodes.hpp"
#include <NFSEngine.h>

class CubeControl : public NFSEngine::Component
{
public:
    CubeControl(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { };

    std::string GetName() const override { return "CubeControl"; };

protected:
    virtual void OnAwake() override { p_Transform = m_Owner->GetComponent<NFSEngine::Transform>(); }

    virtual void OnUpdate(NFSEngine::DeltaTime deltaTime) override
    {
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::A))
        {
            p_Transform->Move({ -1 * deltaTime, 0, 0 });
        }
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::D))
        {
            p_Transform->Move({ 1 * deltaTime, 0, 0 });
        }
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::W))
        {
            p_Transform->Move({ 0, 1 * deltaTime, 0 });
        }
        if (NFSEngine::Input::IsKeyPressed(NFSEngine::Key::S))
        {
            p_Transform->Move({ 0, -1 * deltaTime, 0 });
        }
    }

private:
    NFSEngine::Transform* p_Transform;
};