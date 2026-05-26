#pragma once 

#include <NFSEngine.h>
#include "Events/NotePlayedEvent.hpp"

#include "Components/CubeMesh.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PhysicsComponents.hpp" 

class RhythmPlatform : public NFSEngine::Component {
public:
    std::string TargetPattern = "BassPatternPrototype";

    bool StartsActive = true;

    RhythmPlatform(NFSEngine::GameObject* owner) : Component(owner) {}

    std::string GetName() const override { return "RhythmPlatform"; }

    virtual void OnAwake() override {
        m_IsActive = StartsActive;

        m_OriginalScale = GetOwner()->GetTransform()->GetScale();

        ApplyState();
    }

    void OnEvent(NFSEngine::Event& e){
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&RhythmPlatform::OnNotePlayed, this, std::placeholders::_1));
    }

private:
    bool m_IsActive = true;
    glm::vec3 m_OriginalScale{ 1.0f };

    bool OnNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (e.GetPatternName() != TargetPattern) return false;

        m_IsActive = !m_IsActive;
        ApplyState();

        return false;
    }

    void ApplyState() {       
        if (m_IsActive) {
            GetOwner()->GetTransform()->SetScale(m_OriginalScale);
        }
        else {
            GetOwner()->GetTransform()->SetScale(glm::vec3(0.0f));
        }
    }
};