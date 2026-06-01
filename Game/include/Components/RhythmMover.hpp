#pragma once 

#include <NFSEngine.h>
#include "Events/NotePlayedEvent.hpp"

class RhythmMover : public NFSEngine::Component {
public:
	std::string TargetTrack = "Piano";

	float JumpHeight = 2.0f;
	float FallSpeed = 5.0f;

	RhythmMover(NFSEngine::GameObject* owner) : Component(owner) {}

	std::string GetName() const override { return "RhythmMover"; }

	void OnEvent(NFSEngine::Event& e) {
		NFSEngine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NFSEngine::NotePlayedEvent>(std::bind(&RhythmMover::OnNotePlayed, this, std::placeholders::_1));
	}

	void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
		if (m_CurrentJumpOffset > 0.0f) {
			m_CurrentJumpOffset -= FallSpeed * deltaTime.GetSeconds();

			if (m_CurrentJumpOffset < 0.0f) {
				m_CurrentJumpOffset = 0.0f;
			}

			auto transform = GetOwner()->GetTransform();
			glm::vec3 pos = m_BasePosition;
			pos.y += m_CurrentJumpOffset;
			transform->SetPosition(pos);
		}
	}

	virtual void OnAwake() override {
		m_BasePosition = GetOwner()->GetTransform()->GetPosition();
	}

private:
	glm::vec3 m_BasePosition{ 0.0f };
	float m_CurrentJumpOffset = 0.0f;

	bool OnNotePlayed(NFSEngine::NotePlayedEvent& e) {
		if (e.GetTrackName() != TargetTrack) return false;

		m_CurrentJumpOffset = JumpHeight;

		return false;
	}
};