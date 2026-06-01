#pragma once

#include <NFSEngine.h>
#include "Aura/AuraManager.hpp"
#include "Managers/ScoreManager.hpp"
#include "Managers/LivesManager.hpp"

class HUDComponent : public NFSEngine::Component {
public:
    HUDComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) {
        m_Canvas = new NFSEngine::Canvas();
    }

    ~HUDComponent() override {
        if (m_Canvas) delete m_Canvas;
    }

    std::string GetName() const override { return "HUDComponent"; }

    void RenderUI() {
        if (m_Canvas) {
            NFSEngine::UIRenderer::Begin();
            m_Canvas->Draw();
            NFSEngine::UIRenderer::End();
        }
    }

private:
    NFSEngine::Canvas* m_Canvas = nullptr;

    NFSEngine::UIObject* m_ScoreLabel = nullptr;
    NFSEngine::UIObject* m_AuraLabel = nullptr;
    std::vector<NFSEngine::UIObject*> m_Hearts;

    size_t m_AuraEventId = 0;
    size_t m_ScoreEventId = 0;
    size_t m_LivesEventId = 0;

protected:
    void OnAwake() override {
        if (m_Canvas == nullptr) {
            m_Canvas = new NFSEngine::Canvas();
        }
    }

    void OnStart() override {

        InitScoreUI();
        InitHeartsUI();
        InitAuraUI();

        if (AuraManager::Instance) {
            UpdateAuraText(AuraManager::Instance->CurrentAura);
            m_AuraEventId
                = AuraManager::Instance->OnAuraChanged.AddListener([this](AuraType newAura) { this->UpdateAuraText(newAura); });
        }

        if (ScoreManager::Instance) {
            UpdateScoreText(ScoreManager::Instance->GetScore());
            m_ScoreEventId
                = ScoreManager::Instance->OnScoreChanged.AddListener([this](int score) { this->UpdateScoreText(score); });
        }

        if (LivesManager::Instance) {
            UpdateHeartVisuals(LivesManager::Instance->GetLives());
            m_LivesEventId
                = LivesManager::Instance->OnLivesChanged.AddListener([this](int lives) { this->UpdateHeartVisuals(lives); });
        }
    }

    void OnFixedUpdate(NFSEngine::DeltaTime deltaTime) override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_Canvas) {
            m_Canvas->Update();
        }
    }

    void OnEnable() override { }

    void OnDisable() override {
        if (AuraManager::Instance && m_AuraEventId != 0) {
            AuraManager::Instance->OnAuraChanged.RemoveListener(m_AuraEventId);
            m_AuraEventId = 0;
        }
        if (ScoreManager::Instance && m_ScoreEventId != 0) {
            ScoreManager::Instance->OnScoreChanged.RemoveListener(m_ScoreEventId);
            m_ScoreEventId = 0;
        }
        if (LivesManager::Instance && m_LivesEventId != 0) {
            LivesManager::Instance->OnLivesChanged.RemoveListener(m_LivesEventId);
            m_LivesEventId = 0;
        }
    }

private:
    void InitScoreUI() {
        NFSEngine::UI::LabelParameters labelParams;
        labelParams.position = glm::vec3(1500, 100, 2.0f);
        labelParams.text = "SCORE: 0";
        labelParams.color = glm::vec4(1.0f);
        m_ScoreLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);

        NFSEngine::UI::ImageParameters scoreBgParams;
        scoreBgParams.position = glm::vec3(1500, 100, 1.0f);
        scoreBgParams.width = 400;
        scoreBgParams.height = 100;
        scoreBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
        NFSEngine::UI::Image(*m_Canvas, scoreBgParams);
    }
    void InitHeartsUI() {
        constexpr int k_MaxLives = 3; // XD?
        NFSEngine::UI::ImageParameters heartBgParams;
        heartBgParams.position = glm::vec3(300, 100, 0.8f);
        heartBgParams.width = 400;
        heartBgParams.height = 100;
        heartBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.45f);
        NFSEngine::UI::Image(*m_Canvas, heartBgParams);

        const float heartSize = 60.0f;
        const float heartSpacing = 120.0f;
        const float heartStartX = 195.0f;
        const float heartY = 100.0f;

        m_Hearts.reserve(k_MaxLives);
        for (int i = 0; i < k_MaxLives; ++i) {
            NFSEngine::UI::ImageParameters heartParams;
            heartParams.position = glm::vec3(heartStartX + i * heartSpacing, heartY, 1.0f);
            heartParams.width = heartSize;
            heartParams.height = heartSize;
            heartParams.color = glm::vec4(0.9f, 0.1f, 0.15f, 1.0f);
            m_Hearts.push_back(&NFSEngine::UI::Image(*m_Canvas, heartParams));
        }
    }
    void InitAuraUI() {
        NFSEngine::UI::ImageParameters auraBgParams;
        auraBgParams.position = glm::vec3(960, 100, 1.0f);
        auraBgParams.width = 400;
        auraBgParams.height = 100;
        auraBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
        NFSEngine::UI::Image(*m_Canvas, auraBgParams);

        NFSEngine::UI::LabelParameters labelParams;
        labelParams.position = glm::vec3(960, 100, 2.0f);
        labelParams.text = "AURA: NONE";
        labelParams.color = glm::vec4(1.0f);
        m_AuraLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);
    }

    void UpdateScoreText(int newScore) {
        if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>()) {
            m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString = "SCORE: " + std::to_string(newScore);
        }
    }
    void UpdateHeartVisuals(int currentLives) {
        for (size_t i = 0; i < m_Hearts.size(); ++i) {
            if (!m_Hearts[i]) continue;

            auto* img = m_Hearts[i]->GetComponent<NFSEngine::ImageComponent>();
            if (!img) continue;

            if (static_cast<int>(i) < currentLives) {
                img->Color = glm::vec4(0.9f, 0.1f, 0.15f, 1.0f);
            } else {
                img->Color = glm::vec4(0.3f, 0.3f, 0.3f, 0.4f);
            }
        }
    }
    void UpdateAuraText(AuraType newAura) {
        if (!m_AuraLabel || !m_AuraLabel->HasComponent<NFSEngine::TextComponent>()) return;

        std::string auraText = "AURA: ";
        switch (newAura) {
        case AuraType::First:
            auraText += "PIANO";
            break;
        case AuraType::Second:
            auraText += "BASS";
            break;
        case AuraType::Third:
            auraText += "GUITAR";
            break;
        default:
            auraText += "UNKNOWN";
            break;
        }
        m_AuraLabel->GetComponent<NFSEngine::TextComponent>()->TextString = auraText;
    }
};
