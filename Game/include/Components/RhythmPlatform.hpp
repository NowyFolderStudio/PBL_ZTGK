#pragma once

#include <NFSEngine.h>

#include "Components/ScaleAnimator.hpp"

class RhythmPlatform : public NFSEngine::Component {
public:
    std::string TargetTrack = "Bass";
    bool StartsActive = true;

    RhythmPlatform(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "RhythmPlatform"; }

    virtual void OnAwake() override { Initialize(); }

    void OnEvent(NFSEngine::Event& e) {
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&RhythmPlatform::OnNotePlayed, this, std::placeholders::_1));
    }

private:
    bool m_IsActive = true;
    bool m_Initialized = false;
    glm::vec3 m_OriginalScale { 1.0f };

    ScaleAnimator* m_Animator = nullptr;

    void Initialize() {
        if (!m_Initialized) {
            m_Animator = GetOwner()->GetComponent<ScaleAnimator>();

            m_OriginalScale = GetOwner()->GetTransform()->GetScale();
            m_IsActive = StartsActive;
            m_Initialized = true;
            ApplyState();
        }
    }

    bool OnNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (e.GetTrackName() != TargetTrack) return false;

        Initialize();

        m_IsActive = !m_IsActive;
        ApplyState();

        return false;
    }

    void ApplyState() {
        auto transform = GetOwner()->GetTransform();

        if (m_IsActive) {
            m_Animator->SetTargetScale(glm::vec3(1.0f));
        } else {
            m_Animator->SetTargetScale(glm::vec3(0.1f));
        }
    }
};