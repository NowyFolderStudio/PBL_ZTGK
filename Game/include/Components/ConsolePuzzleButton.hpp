#pragma once
#include "Components/ActivatebleComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Renderer/Material.hpp"
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <string>

class ConsolePuzzle;

class ConsolePuzzleButton : public NFSEngine::Component {
public:
    ConsolePuzzleButton(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "Console Puzzle Button"; }

    int buttonID;
    ConsolePuzzle* puzzleManager = nullptr;

    glm::vec3 BaseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float BaseStrength = 0.3f;

    glm::vec3 ActiveColor = glm::vec3(1.0f, 0.0f, 0.0f);
    float ActiveStrength = 5.0f;

    float CooldownTime = 0.2f;
    float ReleaseDelay = 0.15f;

    void OnPlayerStepped();

    void SetMaterialEmission(const glm::vec3& color, float strength) {
        if (m_Material) {
            m_Material->EmissiveColor = color;
            m_Material->EmissiveStrength = strength;
        }
    }

    void ResetMaterialEmission() { SetMaterialEmission(BaseColor, BaseStrength); }

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override; // Dodano OnUpdate

private:
    std::shared_ptr<NFSEngine::Material> m_Material = nullptr;

    bool m_IsPressed = false;
    bool m_IsCurrentlyColliding = false;
    glm::vec3 m_BasePosition { 0.0f };

    float m_CurrentCooldown = 0.0f;
    float m_TimeSinceLastCollision = 0.0f;

    NFSEngine::Transform* m_Transform = nullptr;
};