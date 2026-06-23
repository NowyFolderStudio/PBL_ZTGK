#pragma once

#include "Core/Log.hpp"
#include <NFSEngine.h>
#include <miniaudio/miniaudio.h>
#include <string>
#include <vector>
#include <cmath>
#include "Core/Scene.hpp"
#include "Core/Tags.hpp"
#include "PianoKeyTrigger.hpp"
#include "PianoPowerButton.hpp"

class InteractivePiano : public NFSEngine::Component {
public:
    bool IsPoweredOn = false;

    InteractivePiano(NFSEngine::GameObject* owner)
        : Component(owner) {
    }

    ~InteractivePiano() override {
        for (auto& sound : m_KeySounds) {
            ma_sound_uninit(&sound);
        }
    }

    std::string GetName() const override { return "InteractivePiano"; }

    void LoadPiano(const std::string& baseAudioPath) {
        m_KeySounds.resize(25);

        for (int i = 0; i < 25; i++) {
            ma_result result = ma_sound_init_from_file(NFSEngine::AudioEngine::GetEngine(), baseAudioPath.c_str(), 0, NULL, NULL, &m_KeySounds[i]);

            if (result == MA_SUCCESS) {
                float pitchMultiplier = std::pow(2.0f, i / 12.0f);
                ma_sound_set_pitch(&m_KeySounds[i], pitchMultiplier);
            }
        }

        m_IsLoaded = true;
    }

    void PlayKey(int keyIndex) {
        if (!m_IsLoaded || keyIndex < 0 || keyIndex >= 25) return;

        ma_sound_seek_to_pcm_frame(&m_KeySounds[keyIndex], 0);
        ma_sound_start(&m_KeySounds[keyIndex]);
    }

    void SetPowerState(bool isOn) {
        IsPoweredOn = isOn;

        if (m_BlackKeyColliders) {
            auto transform = m_BlackKeyColliders->GetTransform();
            if (isOn) {
                transform->SetPosition(m_BlackKeysBasePos);
            }
            else {
                glm::vec3 pos = m_BlackKeysBasePos;
                pos.y += 0.094f;
                transform->SetPosition(pos);
            }
        }

        if (m_WhiteKeyCollider) {
            if (isOn) {
                m_WhiteKeyCollider->SetActive(false);
            }
            else {
                m_WhiteKeyCollider->SetActive(true);
            }
        }
    }

protected:
    void OnStart() override {
        NFSEngine::GameObject* player = GetOwner()->GetScene()->FindWithTag(NFSEngine::Tags::Player);

        if (!player) {
        }

        int keysFound = 0;

        for (const auto& go : GetOwner()->GetScene()->GetAllGameObjects()) {
            NFSEngine::GameObject* childObj = go.get();
            std::string name = childObj->name;

            if (childObj->GetTransform()->GetParent() == GetOwner()->GetTransform()) {

                size_t keyPos = name.find("Key");
                if (keyPos != std::string::npos && name != "BlackKeyColliders" && name != "WhiteKeyCollider") {
                    try {
                        std::string numStr = name.substr(keyPos + 3);
                        int keyNum = std::stoi(numStr);
                        int keyIndex = keyNum - 1;

                        if (keyIndex >= 0 && keyIndex < 25) {
                            auto& trigger = childObj->AddComponent<PianoKeyTrigger>();
                            trigger.KeyIndex = keyIndex;
                            trigger.MainPiano = this;
                            trigger.TargetPlayer = player;
                            trigger.SetBasePosition(childObj->GetTransform()->GetPosition());

                            m_ManagedTriggers.push_back(&trigger);
                            keysFound++;
                        }
                    }
                    catch (...) {}
                }

                if (name == "Button") {
                    auto& powerBtn = childObj->AddComponent<PianoPowerButton>();
                    powerBtn.MainPiano = this;
                    powerBtn.TargetPlayer = player;
                    m_PowerButton = &powerBtn;
                }

                if (name == "BlackKeyColliders") {
                    m_BlackKeyColliders = childObj;
                    m_BlackKeysBasePos = childObj->GetTransform()->GetPosition();
                }

                if (name == "WhiteKeyCollider") {
                    m_WhiteKeyCollider = childObj;
                }
            }
        }

        SetPowerState(false);
    }

public:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        for (auto* trigger : m_ManagedTriggers) {
            if (trigger) trigger->OnUpdate(deltaTime);
        }

        if (m_PowerButton) {
            m_PowerButton->OnUpdate(deltaTime);
        }
    }

private:
    std::vector<ma_sound> m_KeySounds;
    std::vector<PianoKeyTrigger*> m_ManagedTriggers;
    PianoPowerButton* m_PowerButton = nullptr;

    NFSEngine::GameObject* m_BlackKeyColliders = nullptr;
    glm::vec3 m_BlackKeysBasePos{ 0.0f };
    NFSEngine::GameObject* m_WhiteKeyCollider = nullptr;

    bool m_IsLoaded = false;
};


inline void PianoKeyTrigger::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (!MainPiano || !MainPiano->IsPoweredOn) return;

    if (m_CurrentCooldown > 0.0f) {
        m_CurrentCooldown -= deltaTime.GetSeconds();
    }

    if (m_IsCurrentlyColliding) {
        m_TimeSinceLastCollision = 0.0f;

        if (!m_IsPressed && m_CurrentCooldown <= 0.0f) {
            m_IsPressed = true;
            MainPiano->PlayKey(KeyIndex);

            auto transform = GetOwner()->GetTransform();
            glm::vec3 pos = m_BasePosition;
            pos.y -= 0.08f;
            transform->SetPosition(pos);
        }
    }
    else {
        m_TimeSinceLastCollision += deltaTime.GetSeconds();

        if (m_IsPressed && m_TimeSinceLastCollision > ReleaseDelay) {
            m_IsPressed = false;
            m_CurrentCooldown = CooldownTime;

            auto transform = GetOwner()->GetTransform();
            transform->SetPosition(m_BasePosition);
        }
    }
}

inline void PianoPowerButton::OnStart() {
    auto renderComponent = GetOwner()->GetComponent<NFSEngine::ModelComponent>();
    if (renderComponent) {
        m_Material = renderComponent->GetMaterial(0);
    }

    m_BasePosition = GetOwner()->GetTransform()->GetPosition();

    auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
    if (collider) {
        collider->IsTrigger = true;

        collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
            if (TargetPlayer && other == TargetPlayer) {

                if (m_Cooldown <= 0.0f) {
                    m_IsOn = !m_IsOn;

                    if (MainPiano) {
                        MainPiano->SetPowerState(m_IsOn);
                    }

                    m_Cooldown = 1.0f;

                    auto transform = GetOwner()->GetTransform();
                    if (m_IsOn) {
                        glm::vec3 pos = m_BasePosition;
                        pos.y -= 0.1f;
                        transform->SetPosition(pos);
                    }
                    else {
                        transform->SetPosition(m_BasePosition);
                    }
                }
            }
            };
    }
}

inline void PianoPowerButton::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Cooldown > 0.0f) {
        m_Cooldown -= deltaTime.GetSeconds();
    }

    if (m_IsOn) {
        SetEmission(OnColor, OnStrength);
    }
    else {
        m_Time += deltaTime.GetSeconds();
        float pulse = (std::sin(m_Time * 4.0f) + 1.0f) / 2.0f;
        SetEmission(OffColor, OffStrength * pulse);
    }
}

inline void PianoPowerButton::SetEmission(const glm::vec3& color, float strength) {
    if (m_Material) {
        m_Material->EmissiveColor = color;
        m_Material->EmissiveStrength = strength;
    }
}