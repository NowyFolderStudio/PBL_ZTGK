#pragma once

#include <NFSEngine.h>
#include <string>
#include "Components/MusicDirector.hpp"

class MusicTriggerComponent : public NFSEngine::Component {
public:
	std::string TargetTrack = "Bass";

	explicit MusicTriggerComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

	std::string GetName() const override { return "MusicTriggerComponent"; }
	
protected:
	void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;

        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Triggered) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            m_Triggered = true;

            auto* directorObj = m_Owner->GetScene()->FindGameObject("MusicDirector");
            if (directorObj) {
                auto* director = directorObj->GetComponent<MusicDirector>();
                if (director) {
                    director->AdvanceTrackStage(TargetTrack);
                }
            }

            m_Owner->SetActive(false);
        };
	}

private:
    bool m_Triggered = false;
};