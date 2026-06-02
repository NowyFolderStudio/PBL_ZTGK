#pragma once
#include <NFSEngine.h>
#include "Events/NotePlayedEvent.hpp"
#include <vector>

enum class TileState {
    Idle,
    Popping,
    Returning
};

struct TileData {
    NFSEngine::Transform* Transform = nullptr;
    NFSEngine::RigidBody3DComponent* RigidBody = nullptr;
    glm::vec3 BasePosition { 0.0f };
    float Extension = 0.0f;
    TileState State = TileState::Idle;
};

class DancingWall : public NFSEngine::Component {
public:
    std::string TargetTrack = "Bass";

    glm::vec3 PopOutOffset = { 0.0f, 0.0f, -20.0f };
    float PopOutSpeed = 15.0f;
    float ReturnSpeed = 5.0f;

    DancingWall(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "DancingWall"; }

    virtual void OnAwake() override {
        size_t groupCount = GetOwner()->GetTransform()->GetChildCount();

        for (size_t g = 0; g < groupCount; g++) {
            auto* groupTransform = GetOwner()->GetTransform()->GetChild(g);
            std::vector<TileData> groupTiles;

            size_t tileCount = groupTransform->GetChildCount();

            for (size_t t = 0; t < tileCount; t++) {
                auto* tileTransform = groupTransform->GetChild(t);

                TileData td;
                td.Transform = tileTransform;

                td.RigidBody = tileTransform->GetOwner()->GetComponent<NFSEngine::RigidBody3DComponent>();

                if (!td.RigidBody) {
                    td.RigidBody = &tileTransform->GetOwner()->AddComponent<NFSEngine::RigidBody3DComponent>();
                }

                td.RigidBody->IsKinematic = true;
                td.RigidBody->UseGravity = false;

                td.BasePosition = tileTransform->GetPosition();
                groupTiles.push_back(td);
            }
            m_Groups.push_back(groupTiles);
        }
    }

    void OnEvent(NFSEngine::Event& e) {
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&DancingWall::OnNotePlayed, this, std::placeholders::_1));
    }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        float dt = deltaTime.GetSeconds();

        for (auto& group : m_Groups) {
            for (auto& tile : group) {

                if (!tile.RigidBody) continue;

                glm::vec3 popOutVelocity = PopOutOffset * PopOutSpeed;
                glm::vec3 returnVelocity = PopOutOffset * ReturnSpeed;

                glm::vec3 direction = glm::normalize(PopOutOffset);
                glm::vec3 currentPos = tile.Transform->GetPosition();

                if (tile.State == TileState::Popping) {
                    tile.RigidBody->Velocity = popOutVelocity;

                    float currentDist = glm::length(currentPos - tile.BasePosition);
                    float targetDist = glm::length(PopOutOffset);

                    if (currentDist >= targetDist) {
                        tile.State = TileState::Returning;
                    }

                } else if (tile.State == TileState::Returning) {
                    tile.RigidBody->Velocity = -returnVelocity;

                    glm::vec3 toBase = tile.BasePosition - currentPos;
                    if (glm::dot(direction, toBase) > 0.0f) {
                        tile.RigidBody->Velocity = glm::vec3(0.0f);
                        tile.Transform->SetPosition(tile.BasePosition);
                        tile.State = TileState::Idle;
                    }

                } else if (tile.State == TileState::Idle) {
                    tile.RigidBody->Velocity = glm::vec3(0.0f);
                }
            }
        }
    }

private:
    std::vector<std::vector<TileData>> m_Groups;
    int m_CurrentStep = 0;

    bool OnNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (e.GetTrackName() != TargetTrack) return false;

        for (auto& group : m_Groups) {
            if (m_CurrentStep < group.size()) {
                group[m_CurrentStep].State = TileState::Popping;
            }
        }

        m_CurrentStep = (m_CurrentStep + 1) % 4;

        return false;
    }
};