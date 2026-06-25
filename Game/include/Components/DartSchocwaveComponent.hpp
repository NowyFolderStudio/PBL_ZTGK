#pragma once

#include <NFSEngine.h>
#include "Components/Managers/LivesManager.hpp"
#include <cmath>

class DartShockwaveComponent : public NFSEngine::Component {
public:
    DartShockwaveComponent(NFSEngine::GameObject* owner) : NFSEngine::Component(owner) {}

    std::string GetName() const override { return "DartShockwaveComponent"; }

    float MaxRadius = 60.0f;
    float ExpansionSpeed = 25.0f;
    float WaveThickness = 2.0f;
    float JumpClearanceY = 3.0f;

    NFSEngine::GameObject* PlayerRef = nullptr;
    std::shared_ptr<NFSEngine::Material> WaveMaterial = nullptr;

    void Fire() {
        m_CurrentRadius = 0.0f;
        m_IsActive = true;
        m_HasHitPlayer = false;


        if (!PlayerRef) {
            PlayerRef = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);
        }

        GetOwner()->GetTransform()->SetScale(glm::vec3(0.1f));
        GetOwner()->SetActive(true);
    }

protected:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (!m_IsActive) return;

        m_CurrentRadius += ExpansionSpeed * deltaTime.GetSeconds();

        GetOwner()->GetTransform()->SetScale(glm::vec3(m_CurrentRadius, 1.0f, m_CurrentRadius));

        if (WaveMaterial) {
            float alpha = 1.0f - (m_CurrentRadius / MaxRadius);
            WaveMaterial->SetFloat("u_Alpha", std::max(0.0f, alpha));
        }

        if (PlayerRef && !m_HasHitPlayer) {
            glm::vec3 waveCenter = GetOwner()->GetTransform()->GetPosition();
            glm::vec3 playerPos = PlayerRef->GetTransform()->GetPosition();

            if (playerPos.y <= waveCenter.y + JumpClearanceY) {

                float dx = playerPos.x - waveCenter.x;
                float dz = playerPos.z - waveCenter.z;
                float distToPlayer = std::sqrt(dx * dx + dz * dz);

                if (distToPlayer > (m_CurrentRadius - WaveThickness) && distToPlayer < (m_CurrentRadius + WaveThickness)) {
                    if (LivesManager::Instance) {
                        LivesManager::Instance->LoseHeart();
                        m_HasHitPlayer = true;
                        NFS_CORE_INFO("Gracz oberwal od fali uderzeniowej!");
                    }
                }
            }
        }

        if (m_CurrentRadius >= MaxRadius) {
            m_IsActive = false;
            GetOwner()->SetActive(false);
        }
    }

private:
    float m_CurrentRadius = 0.0f;
    bool m_IsActive = false;
    bool m_HasHitPlayer = false;
};