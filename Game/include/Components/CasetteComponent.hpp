#pragma once

#include <NFSEngine.h>

#include "Components/Aura/AuraManager.hpp"
#include "Components/MusicDirector.hpp"

class CasetteComponent : public NFSEngine::Component {
public:
    std::vector<std::string> TracksToUnlock;

    explicit CasetteComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) { }

    std::string GetName() const override { return "CasetteComponent"; }

protected:
    void OnStart() override {
        auto* collider = m_Owner->GetComponent<NFSEngine::ColliderComponent>();
        if (!collider) return;
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (m_Collected) return;
            if (!other->CompareTag(NFSEngine::Tags::Player)) return;

            m_Collected = true;

            if (AuraManager::Instance) {
                AuraManager::Instance->UnlockNextAura();
            }

            auto* directorObj = m_Owner->GetScene()->FindGameObject("MusicDirector");
            if (directorObj) {
                auto* director = directorObj->GetComponent<MusicDirector>();
                if (director) {
                    for (const auto& track : TracksToUnlock) {
                        director->UnlockTrack(track);
                    }
                }
            }

            m_Owner->SetActive(false);
        };
    }

private:
    bool m_Collected = false;
};
