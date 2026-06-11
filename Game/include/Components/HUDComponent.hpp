#pragma once

#include <NFSEngine.h>
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include "Aura/AuraManager.hpp"
#include "Components/Transform.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Managers/ScoreManager.hpp"
#include "Managers/LivesManager.hpp"
#include "Renderer/Texture.hpp"
#include "UI/UIComponents.hpp"
#include "UI/UIFactory.hpp"
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
    std::vector<NFSEngine::UIObject*> m_FullNotes;
    size_t m_NumberOfNotes = 9;

    size_t m_AuraEventId = 0;
    size_t m_ScoreEventId = 0;
    size_t m_LivesEventId = 0;

    const std::vector<AuraType> m_AuraOrder = { AuraType::First, AuraType::Second };

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

        float originalWidth = 462;
        float originalHeight = 591;
        float aspectRatio = originalWidth / originalHeight;

        float noteHeight = 100;
        float noteWidth = noteHeight * aspectRatio;
        float targetWidth = 500;
        glm::vec3 firstPosition = { (1920 - targetWidth) / 2.0f, 1000.0f, 1.0f };

        float stepX = (targetWidth - noteWidth) / (m_NumberOfNotes - 1);
        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;
        auto bgTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_background.png", texParam);
        auto emptyTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_empty_v2.png", texParam);
        auto fullTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_full.png", texParam);
        auto fullExtraTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_full_extra.png", texParam);

        for (int i = 0; i < m_NumberOfNotes; ++i) {
            NFSEngine::UI::ImageParameters noteParameters;

            glm::vec3 currentPosition = firstPosition;
            currentPosition.x += i * stepX;

            noteParameters.position = currentPosition;
            noteParameters.width = noteWidth;
            noteParameters.height = noteHeight;
            noteParameters.color = glm::vec4(1, 1, 1, 1.0f);
            noteParameters.texture = bgTex;

            NFSEngine::UI::Image(*m_Canvas, noteParameters);
        }
        for (int i = 0; i < m_NumberOfNotes; ++i) {
            NFSEngine::UI::ImageParameters noteParameters;

            glm::vec3 currentPosition = firstPosition;
            currentPosition.x += i * stepX;

            noteParameters.position = currentPosition + glm::vec3(0, 0, 1 + (1 * i));
            noteParameters.width = noteWidth;
            noteParameters.height = noteHeight;
            noteParameters.color = glm::vec4(1, 1, 1, 1.0f);
            noteParameters.texture = emptyTex;

            NFSEngine::UI::Image(*m_Canvas, noteParameters);
        }
        m_FullNotes.reserve(m_NumberOfNotes);
        for (int i = 0; i < m_NumberOfNotes; ++i) {
            NFSEngine::UI::ImageParameters noteParameters;

            glm::vec3 currentPosition = firstPosition;
            currentPosition.x += i * stepX;

            noteParameters.position = currentPosition + glm::vec3(0, 0, 2 + (1 * i));
            noteParameters.width = noteWidth;
            noteParameters.height = noteHeight;
            noteParameters.color = glm::vec4(1, 1, 1, 0.0f);
            if (i % 3 == 2)
                noteParameters.texture = fullExtraTex;
            else
                noteParameters.texture = fullTex;

            m_FullNotes.push_back(&NFSEngine::UI::Image(*m_Canvas, noteParameters));
        }
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

    void UpdateScoreText(int newScore) {
        // if (m_ScoreLabel && m_ScoreLabel->HasComponent<NFSEngine::TextComponent>()) {
        //     m_ScoreLabel->GetComponent<NFSEngine::TextComponent>()->TextString = "SCORE: " + std::to_string(newScore);
        // }
        if (!m_FullNotes.empty()) {
            int allCoins = 132;
            float note = 132 / m_NumberOfNotes;
            float howMuchCompleted = newScore / note;
            float lastNote = std::fmod(howMuchCompleted, 1.0f);

            if ((int)howMuchCompleted >= m_NumberOfNotes) {
                for (int i = 0; i < m_NumberOfNotes; i++) {
                    m_FullNotes[i]->GetComponent<NFSEngine::ImageComponent>()->Color.a = 1;
                }
            } else {
                for (int i = 0; i < howMuchCompleted; i++) {
                    m_FullNotes[i]->GetComponent<NFSEngine::ImageComponent>()->Color.a = 1;
                }
                if ((int)howMuchCompleted < m_NumberOfNotes)
                    m_FullNotes[(int)howMuchCompleted]->GetComponent<NFSEngine::ImageComponent>()->Color.a = lastNote;
            }
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
    std::string GetAuraName(AuraType aura) {
        switch (aura) {
        case AuraType::First:
            return "PIANO";
        case AuraType::Second:
            return "BASS";
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
        default:
            return "";
        }
    }

    void UpdateAuraText(AuraType newAura) {
        auto SetText = [&](NFSEngine::UIObject* obj, std::string text, float scale) {
            if (obj && obj->HasComponent<NFSEngine::TextComponent>()) {
                auto* tc = obj->GetComponent<NFSEngine::TextComponent>();
                tc->Color = glm::vec4(1.0f);
                tc->TextString = text;
                tc->Scale = scale;
            }
        };

        SetText(m_CenterNameLabel, GetAuraName(newAura), 1.0f);
        SetText(m_CenterSkillLabel, GetAuraSkill(newAura), 0.7f);

        SetText(m_LeftNameLabel, GetAuraName(AuraType::First), 0.8f);
        SetText(m_LeftSkillLabel, GetAuraSkill(AuraType::First), 0.6f);

        SetText(m_RightNameLabel, GetAuraName(AuraType::Second), 0.8f);
        SetText(m_RightSkillLabel, GetAuraSkill(AuraType::Second), 0.6f);
    }
};