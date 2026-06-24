#pragma once

#include <NFSEngine.h>
#include "Components/FallingDartComponent.hpp"
#include "Components/CubeMesh.hpp"
#include "Events/NotePlayedEvent.hpp"
#include <vector>
#include <string>
#include <random>

struct PendingDartAttack {
    glm::vec3 TargetPosition;
    int RemainingNotes;
    NFSEngine::GameObject* IndicatorInstance;
};

class DartRainAttackComponent : public NFSEngine::Component {
public:
    DartRainAttackComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartRainAttackComponent"; }

    bool IsActive = true;
    std::string TargetTrack = "Kick";
    int NotesDelay = 1;

    float SpawnHeight = 150.0f;
    float MaxSpawnOffset = 15.0f;

    NFSEngine::GameObject* PlayerTransform = nullptr;

    std::vector<std::string> DartTexturePaths = {
        "assets/models/Rzutka/color_rzutka_neonpink.png",
        "assets/models/Rzutka/color_rzutka_orange1.png",
        "assets/models/Rzutka/color_rzutka_silver_green.png",
        "assets/models/Rzutka/color_rzutka_silver_neonpink.png"
    };

protected:
    void OnAwake() override {
        auto dartShader = NFSEngine::Shader::Create("DartShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShaderNew.frag");
        auto dartModel = std::make_shared<NFSEngine::Model>("assets/models/Rzutka/rzutka.fbx");

        auto indicatorShader = NFSEngine::Shader::Create("IndicatorShader", "assets/shaders/lightShader.vert", "assets/shaders/toonShaderNew.frag");
        auto indicatorMaterial = std::make_shared<NFSEngine::Material>();
        indicatorMaterial->AlbedoColor = glm::vec3(1.0f, 0.0f, 0.0f);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> texDist(0, DartTexturePaths.size() - 1);

        for (int i = 0; i < 12; i++) {
            auto* indicatorObj = GetOwner()->GetScene()->CreateGameObject("DartIndicator_" + std::to_string(i));
            indicatorObj->GetTransform()->SetScale(glm::vec3(2.0f, 0.1f, 2.0f));
            indicatorObj->AddComponent<NFSEngine::CubeMesh>(indicatorShader, indicatorMaterial);
            indicatorObj->SetActive(false);
            m_IndicatorPool.push_back(indicatorObj);

            auto* dartObj = GetOwner()->GetScene()->CreateGameObject("PooledDart_" + std::to_string(i));
            dartObj->GetTransform()->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

            auto dartMat = std::make_shared<NFSEngine::Material>();
            std::string randomTexPath = DartTexturePaths[texDist(gen)];
            dartMat->AlbedoMap = NFSEngine::Texture::Create(randomTexPath);

            auto& modelComp = dartObj->AddComponent<NFSEngine::ModelComponent>(dartShader, dartMat);
            modelComp.AddLOD(dartModel, 10000.0f);

            dartObj->AddComponent<FallingDartComponent>();

            auto& boxCol = dartObj->AddComponent<NFSEngine::BoxCollider3DComponent>();
            boxCol.IsTrigger = true;

            dartObj->SetActive(false);
            m_DartPool.push_back(dartObj);
        }
    }

public:
    void OnEvent(NFSEngine::Event& e) {
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&DartRainAttackComponent::HandleNotePlayed, this, std::placeholders::_1));
    }

private:
    std::vector<NFSEngine::GameObject*> m_DartPool;
    std::vector<NFSEngine::GameObject*> m_IndicatorPool;
    std::vector<PendingDartAttack> m_PendingAttacks;

    bool HandleNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (!IsActive || e.GetTrackName() != TargetTrack) return false;
        if (!PlayerTransform) {
            PlayerTransform = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);
            if (!PlayerTransform) return false;
        }
        glm::vec3 playerPos = PlayerTransform->GetTransform()->GetPosition();

        float boardY = GetOwner()->GetTransform()->GetPosition().y;
        glm::vec3 targetPos = glm::vec3(playerPos.x, boardY + 7.85f, playerPos.z);

        NFSEngine::GameObject* freeIndicator = nullptr;
        for (auto* ind : m_IndicatorPool) {
            if (!ind->IsActive()) {
                freeIndicator = ind;
                freeIndicator->GetTransform()->SetPosition(targetPos);
                freeIndicator->SetActive(true);
                break;
            }
        }

        PendingDartAttack attack;
        attack.TargetPosition = targetPos;
        attack.RemainingNotes = NotesDelay;
        attack.IndicatorInstance = freeIndicator;
        m_PendingAttacks.push_back(attack);

        ProcessPendingAttacks();

        return false;
    }

    void ProcessPendingAttacks() {
        for (int i = m_PendingAttacks.size() - 1; i >= 0; i--) {
            m_PendingAttacks[i].RemainingNotes--;

            if (m_PendingAttacks[i].RemainingNotes <= 0) {
                SpawnDart(m_PendingAttacks[i]);
                m_PendingAttacks.erase(m_PendingAttacks.begin() + i);
            }
        }
    }

    void SpawnDart(const PendingDartAttack& attackInfo) {
        float randomX = -MaxSpawnOffset + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MaxSpawnOffset * 2)));
        float randomZ = -MaxSpawnOffset + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MaxSpawnOffset * 2)));

        glm::vec3 spawnPosition = attackInfo.TargetPosition + glm::vec3(randomX, SpawnHeight, randomZ);

        for (auto* dartObj : m_DartPool) {
            if (!dartObj->IsActive()) {
                dartObj->GetTransform()->SetPosition(spawnPosition);

                auto* dartLogic = dartObj->GetComponent<FallingDartComponent>();
                if (dartLogic) {
                    dartLogic->Fire(attackInfo.TargetPosition, attackInfo.IndicatorInstance);
                }
                break;
            }
        }
    }
};