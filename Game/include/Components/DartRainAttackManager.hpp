#pragma once

#include <NFSEngine.h>
#include "Components/FallingDartComponent.hpp"
#include "Components/CubeMesh.hpp"
#include "Events/NotePlayedEvent.hpp"
#include <vector>
#include <string>
#include <random>

class DartIndicatorComponent : public NFSEngine::Component {
public:
    DartIndicatorComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}
    std::string GetName() const override { return "DartIndicatorComponent"; }

    glm::vec3 BaseScale = glm::vec3(1.0f);
    float PulseSpeed = 10.0f;
    float MinScale = 0.8f;
    float MaxScale = 1.2f;
    float Timer = 0.0f;
    float Lifetime = 0.8f;

    std::shared_ptr<NFSEngine::Material> MaterialRef = nullptr;

    void Reset() { Timer = 0.0f; m_Time = 0.0f; }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {

        Timer += deltaTime.GetSeconds();
        if (Timer >= Lifetime) {
            GetOwner()->Destroy();
            return;
        }

        m_Time += deltaTime.GetSeconds();
        float sineValue = (std::sin(m_Time * PulseSpeed) + 1.0f) * 0.5f;

        float currentScaleXZ = MinScale + sineValue * (MaxScale - MinScale);
        GetOwner()->GetTransform()->SetScale(glm::vec3(currentScaleXZ * BaseScale.x, BaseScale.y, currentScaleXZ * BaseScale.z));

        if (MaterialRef) {
            float emissiveStrength = 1.0f + (sineValue * 4.0f);
            MaterialRef->SetVec3("u_EmissiveColor", glm::vec3(1.0f, 0.0f, 0.0f) * emissiveStrength);
        }
    }

private:
    float m_Time = 0.0f;
};

class DartRainAttackComponent : public NFSEngine::Component {
public:
    DartRainAttackComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartRainAttackComponent"; }

    bool IsActive = true;
    std::string TargetTrack = "Bass";

    float SpawnHeight = 100.0f;
    float MaxSpawnOffset = 45.0f;

    NFSEngine::GameObject* PlayerTransform = nullptr;

    std::vector<std::string> DartTexturePaths = {
        "assets/models/Rzutka/color_rzutka_neonpink.png",
        "assets/models/Rzutka/color_rzutka_orange1.png",
        "assets/models/Rzutka/color_rzutka_silver_green.png",
        "assets/models/Rzutka/color_rzutka_silver_neonpink.png"
    };

    std::shared_ptr<NFSEngine::Shader> m_DartShader;
    std::shared_ptr<NFSEngine::Model> m_DartModel;
    std::shared_ptr<NFSEngine::Shader> m_IndicatorShader;
    std::shared_ptr<NFSEngine::Model> m_IndicatorModel;

protected:
    void OnAwake() override {
        m_DartShader = NFSEngine::Shader::Create("DartShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
        m_DartModel = std::make_shared<NFSEngine::Model>("assets/models/Rzutka/rzutka.fbx");

        m_IndicatorShader = NFSEngine::Shader::Create("IndicatorShader", "assets/shaders/lightShader.vert", "assets/shaders/PBRShader.frag");
        m_IndicatorModel = std::make_shared<NFSEngine::Model>("assets/models/Rzutka/indicator.obj");
    }

public:
    void OnEvent(NFSEngine::Event& e) {
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&DartRainAttackComponent::HandleNotePlayed, this, std::placeholders::_1));
    }

private:
    bool HandleNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (!IsActive || e.GetTrackName() != TargetTrack) return false;

        if (!PlayerTransform) {
            PlayerTransform = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);
            if (!PlayerTransform) return false;
        }

        glm::vec3 playerPos = PlayerTransform->GetTransform()->GetPosition();
        float boardY = GetOwner()->GetTransform()->GetPosition().y;
        glm::vec3 targetPos = glm::vec3(playerPos.x, boardY + 7.85f, playerPos.z);

        auto* indicatorObj = GetOwner()->GetScene()->CreateGameObject("DartIndicator_Dyn");
        indicatorObj->GetTransform()->SetPosition(targetPos);

        auto indicatorMat = std::make_shared<NFSEngine::Material>();
        indicatorMat->AlbedoColor = glm::vec3(1.0f, 0.0f, 0.0f);
        indicatorMat->SetVec3("u_EmissiveColor", glm::vec3(1.0f, 0.0f, 0.0f) * 2.0f);

        auto& indModelComp = indicatorObj->AddComponent<NFSEngine::ModelComponent>(m_IndicatorShader, indicatorMat);
        indModelComp.AddLOD(m_IndicatorModel, 10000.0f);

        auto& indLogic = indicatorObj->AddComponent<DartIndicatorComponent>();
        indLogic.MaterialRef = indicatorMat;


        float randomX = -MaxSpawnOffset + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MaxSpawnOffset * 2)));
        float randomZ = -MaxSpawnOffset + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MaxSpawnOffset * 2)));
        glm::vec3 spawnPosition = targetPos + glm::vec3(randomX, SpawnHeight, randomZ);

        auto* dartObj = GetOwner()->GetScene()->CreateGameObject("PooledDart_Dyn");
        dartObj->GetTransform()->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
        dartObj->GetTransform()->SetPosition(spawnPosition);

        auto dartMat = std::make_shared<NFSEngine::Material>();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> texDist(0, DartTexturePaths.size() - 1);
        dartMat->AlbedoMap = NFSEngine::Texture::Create(DartTexturePaths[texDist(gen)]);

        auto& modelComp = dartObj->AddComponent<NFSEngine::ModelComponent>(m_DartShader, dartMat);
        modelComp.AddLOD(m_DartModel, 10000.0f);

        dartObj->AddComponent<NFSEngine::BoxCollider3DComponent>().IsTrigger = true;

        auto& dartLogic = dartObj->AddComponent<FallingDartComponent>();
        dartLogic.Fire(targetPos, indicatorObj);

        return false;
    }
};