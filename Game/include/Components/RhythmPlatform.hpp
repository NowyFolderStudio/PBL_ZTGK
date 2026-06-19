#pragma once

#include <NFSEngine.h>
#include <glm/common.hpp>
#include "Components/ModelComponent.hpp"
#include "Components/PhysicsComponents.hpp"
#include "Core/DeltaTime.hpp"
#include "Core/Log.hpp"
#include "Events/NotePlayedEvent.hpp"

#include "Components/ScaleAnimator.hpp"

class RhythmPlatform : public NFSEngine::Component {
public:
    std::string TargetTrack = "Bass";
    bool StartsActive = true;

    RhythmPlatform(NFSEngine::GameObject* owner)
        : Component(owner) { }

    std::string GetName() const override { return "RhythmPlatform"; }

    void OnAwake() override {
        m_Collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
        Initialize();
    }

    void OnEvent(NFSEngine::Event& e) {
        NFSEngine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&RhythmPlatform::OnNotePlayed, this, std::placeholders::_1));
    }

protected:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_IsActive) {
            m_Opacity += deltaTime * m_AnimationSpeed;
        } else {
            m_Opacity -= deltaTime * m_AnimationSpeed;
        }
        m_Opacity = glm::min(glm::max(m_Opacity, m_MinOpacity), m_MaxOpacity);
        m_Model->GetMaterial(0)->SetFloat("u_Opacity", m_Opacity);
        m_Collider->IsTrigger = (m_Opacity <= m_MinOpacity);
    }

private:
    bool m_IsActive = true;
    bool m_Initialized = false;
    float m_Opacity = 0;
    float m_MaxOpacity = 1.0;
    float m_MinOpacity = 0.05;
    float m_AnimationSpeed = 5;

    NFSEngine::ModelComponent* m_Model = nullptr;
    NFSEngine::ColliderComponent* m_Collider = nullptr;

    void Initialize() {
        if (!m_Initialized) {
            m_Model = GetOwner()->GetComponent<NFSEngine::ModelComponent>();
            m_IsActive = StartsActive;
            m_Initialized = true;
        }
    }

    bool OnNotePlayed(NFSEngine::NotePlayedEvent& e) {
        if (e.GetTrackName() != TargetTrack) return false;

        Initialize();
        m_IsActive = !m_IsActive;
        return false;
    }
};