#pragma once

#include <NFSEngine.h>
#include "Aura/AuraManager.hpp"
#include "Managers/ScoreManager.hpp"
#include "Managers/LivesManager.hpp"
#include <vector>
#include <string>

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

    NFSEngine::UIObject* m_CenterNameLabel = nullptr;
    NFSEngine::UIObject* m_CenterSkillLabel = nullptr;

    NFSEngine::UIObject* m_LeftKeyLabel = nullptr;
    NFSEngine::UIObject* m_LeftNameLabel = nullptr;
    NFSEngine::UIObject* m_LeftSkillLabel = nullptr;

    NFSEngine::UIObject* m_RightKeyLabel = nullptr;
    NFSEngine::UIObject* m_RightNameLabel = nullptr;
    NFSEngine::UIObject* m_RightSkillLabel = nullptr;

    std::vector<NFSEngine::UIObject*> m_Hearts;

    size_t m_AuraEventId = 0;
    size_t m_ScoreEventId = 0;
    size_t m_LivesEventId = 0;

    const std::vector<AuraType> m_AuraOrder = { AuraType::First, AuraType::Second, AuraType::Third };

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
        labelParams.position = glm::vec3(1720, 100, 2.0f);
        labelParams.text = "SCORE: 0";
        labelParams.color = glm::vec4(1.0f);
        m_ScoreLabel = &NFSEngine::UI::Label(*m_Canvas, labelParams);

        NFSEngine::UI::ImageParameters scoreBgParams;
        scoreBgParams.position = glm::vec3(1720, 100, 1.0f);
        scoreBgParams.width = 300;
        scoreBgParams.height = 100;
        scoreBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
        NFSEngine::UI::Image(*m_Canvas, scoreBgParams);
    }

    void InitHeartsUI() {
        constexpr int k_MaxLives = 3;

        NFSEngine::UI::ImageParameters heartBgParams;
        heartBgParams.position = glm::vec3(220, 100, 0.8f);
        heartBgParams.width = 360;
        heartBgParams.height = 100;
        heartBgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.45f);
        NFSEngine::UI::Image(*m_Canvas, heartBgParams);

        const float heartSize = 60.0f;
        const float heartSpacing = 100.0f;
        const float heartStartX = 120.0f;
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
        const float centerX = 960.0f;
        const float sideOffset = 340.0f;
        const float spacing = 35.0f;

        auto SetupSideBlock = [&](float x, std::string keyText, NFSEngine::UIObject*& key, NFSEngine::UIObject*& name,
                                  NFSEngine::UIObject*& skill) {
            NFSEngine::UI::ImageParameters bg;
            bg.position = glm::vec3(x, 80.0f, 1.0f);
            bg.width = 240;
            bg.height = 120;
            bg.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.4f);
            NFSEngine::UI::Image(*m_Canvas, bg);

            NFSEngine::UI::LabelParameters pKey;
            pKey.position = glm::vec3(x, 80.0f + spacing, 2.0f);
            pKey.text = keyText;
            pKey.scale = 0.6f;
            pKey.color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
            key = &NFSEngine::UI::Label(*m_Canvas, pKey);

            NFSEngine::UI::LabelParameters pName;
            pName.position = glm::vec3(x, 80.0f, 2.0f);
            pName.text = "NONE";
            name = &NFSEngine::UI::Label(*m_Canvas, pName);

            NFSEngine::UI::LabelParameters pSkill;
            pSkill.position = glm::vec3(x, 80.0f - spacing, 2.0f);
            pSkill.text = "(NONE)";
            pSkill.scale = 0.6f;
            skill = &NFSEngine::UI::Label(*m_Canvas, pSkill);
        };

        SetupSideBlock(centerX - sideOffset, "[Q]", m_LeftKeyLabel, m_LeftNameLabel, m_LeftSkillLabel);
        SetupSideBlock(centerX + sideOffset, "[E]", m_RightKeyLabel, m_RightNameLabel, m_RightSkillLabel);

        NFSEngine::UI::ImageParameters centerBg;
        centerBg.position = glm::vec3(centerX, 110.0f, 1.1f);
        centerBg.width = 320;
        centerBg.height = 120;
        centerBg.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.8f);
        NFSEngine::UI::Image(*m_Canvas, centerBg);

        NFSEngine::UI::LabelParameters pName;
        pName.position = glm::vec3(centerX, 125.0f, 2.0f);
        pName.text = "NONE";
        m_CenterNameLabel = &NFSEngine::UI::Label(*m_Canvas, pName);

        NFSEngine::UI::LabelParameters pSkill;
        pSkill.position = glm::vec3(centerX, 95.0f, 2.0f);
        pSkill.text = "(NONE)";
        pSkill.scale = 0.7f;
        m_CenterSkillLabel = &NFSEngine::UI::Label(*m_Canvas, pSkill);
    }
    std::string GetAuraName(AuraType aura) {
        switch (aura) {
        case AuraType::First:
            return "PIANO";
        case AuraType::Second:
            return "BASS";
        case AuraType::Third:
            return "GUITAR";
        default:
            return "UNKNOWN";
        }
    }

    std::string GetAuraSkill(AuraType aura) {
        switch (aura) {
        case AuraType::First:
            return "(DOUBLE JUMP)";
        case AuraType::Second:
            return "(DASH)";
        case AuraType::Third:
            return "(ATTACK)";
        default:
            return "";
        }
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
        int currentIndex = 0;
        int totalAuras = static_cast<int>(m_AuraOrder.size());

        for (int i = 0; i < totalAuras; ++i) {
            if (m_AuraOrder[i] == newAura) {
                currentIndex = i;
                break;
            }
        }

        int leftIndex = (currentIndex - 1 + totalAuras) % totalAuras;
        int rightIndex = (currentIndex + 1) % totalAuras;

        auto SetText = [&](NFSEngine::UIObject* obj, std::string text, float scale) {
            if (obj && obj->HasComponent<NFSEngine::TextComponent>()) {
                auto* tc = obj->GetComponent<NFSEngine::TextComponent>();
                tc->Color = glm::vec4(1.0f);
                tc->TextString = text;
                tc->Scale = scale;
            }
        };

        SetText(m_CenterNameLabel, GetAuraName(m_AuraOrder[currentIndex]), 1.0f);
        SetText(m_CenterSkillLabel, GetAuraSkill(m_AuraOrder[currentIndex]), 0.7f);

        SetText(m_LeftNameLabel, GetAuraName(m_AuraOrder[leftIndex]), 0.8f);
        SetText(m_LeftSkillLabel, GetAuraSkill(m_AuraOrder[leftIndex]), 0.6f);

        SetText(m_RightNameLabel, GetAuraName(m_AuraOrder[rightIndex]), 0.8f);
        SetText(m_RightSkillLabel, GetAuraSkill(m_AuraOrder[rightIndex]), 0.6f);
    }
};