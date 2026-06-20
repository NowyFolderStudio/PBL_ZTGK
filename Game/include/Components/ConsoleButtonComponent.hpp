#pragma once
#include <NFSEngine.h>
#include <glm/glm.hpp>
#include "Components/ModelComponent.hpp"
#include "Components/Transform.hpp"

class ConsoleButtonComponent : public NFSEngine::Component {
public:
    int ButtonIndex = 0;

    std::string SoundFilePath = "";

    glm::vec3 BaseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float BaseStrength = 0.9f;

    glm::vec3 ActiveColor = glm::vec3(1.0f, 0.0f, 0.0f);
    float ActiveStrength = 5.0f;

    float CooldownTime = 0.2f;
    float ReleaseDelay = 0.15f;

    ConsoleButtonComponent(NFSEngine::GameObject* owner)
        : Component(owner) { }

    ~ConsoleButtonComponent() override {
        if (m_SoundLoaded) {
            ma_sound_uninit(&m_ButtonSound);
        }
    }

    std::string GetName() const override { return "ConsoleButtonComponent"; }

protected:
    void OnStart() override {

        auto renderComponent = GetOwner()->GetComponent<NFSEngine::ModelComponent>();
        if (renderComponent) {
            m_Material = renderComponent->GetMaterial(0);
            SetMaterialEmission(BaseColor, BaseStrength);
            m_Transform = GetOwner()->GetTransform();
            m_BasePosition = m_Transform->GetPosition();
        } else {
            renderComponent = GetOwner()->GetTransform()->GetChild(0)->GetOwner()->GetComponent<NFSEngine::ModelComponent>();
            m_Material = renderComponent->GetMaterial(0);
            SetMaterialEmission(BaseColor, BaseStrength);
            m_Transform = GetOwner()->GetTransform()->GetChild(0)->GetOwner()->GetTransform();
            m_BasePosition = m_Transform->GetPosition();
        }

        if (!SoundFilePath.empty()) {
            ma_result result = ma_sound_init_from_file(NFSEngine::AudioEngine::GetEngine(), SoundFilePath.c_str(), 0, NULL, NULL,
                                                       &m_ButtonSound);
            if (result == MA_SUCCESS) {
                m_SoundLoaded = true;
            } else {
                NFS_CORE_ERROR("consolebutton: Couldnt load sound ", SoundFilePath);
            }
        }

        auto* collider = GetOwner()->GetComponent<NFSEngine::ColliderComponent>();
        if (collider) {
            collider->IsTrigger = true;

            collider->OnTriggerEnter = [this](NFSEngine::GameObject* other) {
                if (!other->CompareTag(NFSEngine::Tags::Player)) return;

                m_IsCurrentlyColliding = true;
            };

            collider->OnTriggerExit = [this](NFSEngine::GameObject* other) {
                if (!other->CompareTag(NFSEngine::Tags::Player)) return;

                m_IsCurrentlyColliding = false;
            };
        }
    }

public:
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_CurrentCooldown > 0.0f) {
            m_CurrentCooldown -= deltaTime.GetSeconds();
        }

        if (m_IsCurrentlyColliding) {
            m_TimeSinceLastCollision = 0.0f;

            if (!m_IsPressed && m_CurrentCooldown <= 0.0f) {
                m_IsPressed = true;
                SetMaterialEmission(ActiveColor, ActiveStrength);

                if (m_SoundLoaded) {
                    ma_sound_seek_to_pcm_frame(&m_ButtonSound, 0);
                    ma_sound_start(&m_ButtonSound);
                }

                glm::vec3 pos = m_BasePosition;
                pos.y -= 0.1f;
                m_Transform->SetPosition(pos);
            }
        } else {
            m_TimeSinceLastCollision += deltaTime.GetSeconds();

            if (m_IsPressed && m_TimeSinceLastCollision > ReleaseDelay) {
                m_IsPressed = false;
                m_CurrentCooldown = CooldownTime;
                SetMaterialEmission(BaseColor, BaseStrength);

                m_Transform->SetPosition(m_BasePosition);
            }
        }
    }

private:
    std::shared_ptr<NFSEngine::Material> m_Material = nullptr;

    bool m_IsPressed = false;
    bool m_IsCurrentlyColliding = false;
    glm::vec3 m_BasePosition { 0.0f };

    float m_CurrentCooldown = 0.0f;
    float m_TimeSinceLastCollision = 0.0f;

    ma_sound m_ButtonSound;
    bool m_SoundLoaded = false;
    NFSEngine::Transform* m_Transform;

    void SetMaterialEmission(const glm::vec3& color, float strength) {
        if (m_Material) {
            m_Material->EmissiveColor = color;
            m_Material->EmissiveStrength = strength;
        }
    }
};