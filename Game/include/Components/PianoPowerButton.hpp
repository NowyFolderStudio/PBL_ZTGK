#pragma once
#include <NFSEngine.h>
#include <glm/glm.hpp>
#include "Components/ModelComponent.hpp"

class InteractivePiano;

class PianoPowerButton : public NFSEngine::Component {
public:
    InteractivePiano* MainPiano = nullptr;
    NFSEngine::GameObject* TargetPlayer = nullptr;

    glm::vec3 OffColor = glm::vec3(1.0f, 0.0f, 0.0f);
    float OffStrength = 3.0f;

    glm::vec3 OnColor = glm::vec3(0.0f, 1.0f, 0.0f);
    float OnStrength = 5.0f;

    PianoPowerButton(NFSEngine::GameObject* owner) : Component(owner) {}

    std::string GetName() const override { return "PianoPowerButton"; }

protected:
    void OnStart() override;

public:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void SetEmission(const glm::vec3& color, float strength);

private:
    bool m_IsOn = false;
    glm::vec3 m_BasePosition{ 0.0f };
    float m_Time = 0.0f;
    float m_Cooldown = 0.0f;

    std::shared_ptr<NFSEngine::Material> m_Material = nullptr;
};