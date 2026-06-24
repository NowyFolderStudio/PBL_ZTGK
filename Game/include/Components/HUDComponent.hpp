#pragma once

#include <NFSEngine.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <vector>
#include "Aura/AuraManager.hpp"
#include "Components/Transform.hpp"
#include "Core/Audio/AudioEngine.hpp"
#include "Components/Managers/TutorialManager.hpp"
#include "Managers/ScoreManager.hpp"
#include "Managers/LivesManager.hpp"
#include "Renderer/Texture.hpp"
#include "UI/UIComponents.hpp"
#include "UI/UIFactory.hpp"
#include "Components/Managers/DialogueManager.hpp"
#include "UI/UIObject.hpp"
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

    NFSEngine::UIObject* m_LeftAuraImage = nullptr;
    NFSEngine::UIObject* m_RightAuraImage = nullptr;

    float m_LeftAuraCurrentAlpha = 1.0f;
    float m_LeftAuraTargetAlpha = 1.0f;
    float m_LeftAuraCurrentScale = 1.0f;
    float m_LeftAuraTargetScale = 1.0f;

    float m_RightAuraCurrentAlpha = 1.0f;
    float m_RightAuraTargetAlpha = 1.0f;
    float m_RightAuraCurrentScale = 1.0f;
    float m_RightAuraTargetScale = 1.0f;

    NFSEngine::UIObject* m_CooldownBg = nullptr;
    NFSEngine::UIObject* m_CooldownFill = nullptr;
    NFSEngine::UIObject* m_CooldownFrame = nullptr;
    const float m_MaxCooldownWidth = 459.5f;

    NFSEngine::UIObject* m_Vignette = nullptr;

    glm::vec3 m_FirstVignetteColor = { 0.0f, 0.8f, 0.976f };
    glm::vec3 m_SecondVignetteColor = { 0.6f, 0.9f, 0.37f };

    glm::vec3 m_CurrentVignetteColor = { 0.0f, 0.8f, 0.976f };
    float m_CurrentVignetteAlpha = 1.0f;

    glm::vec3 m_TargetVignetteColor = { 0.0f, 0.8f, 0.976f };
    float m_TargetVignetteAlpha = 1.0f;

    std::shared_ptr<NFSEngine::Texture> m_HeartFullTex = nullptr;
    std::shared_ptr<NFSEngine::Texture> m_HeartEmptyTex = nullptr;

    std::vector<NFSEngine::UIObject*> m_Hearts;
    std::vector<NFSEngine::UIObject*> m_FullNotes;
    std::vector<NFSEngine::UIObject*> m_BgNotes;
    std::vector<NFSEngine::UIObject*> m_EmptyNotes;

    std::vector<float> m_FullNotesBaseWidths;
    std::vector<float> m_FullNotesBaseHeights;

    size_t m_NumberOfNotes = 9;

    size_t m_AuraEventId = 0;
    size_t m_ScoreEventId = 0;
    size_t m_LivesEventId = 0;

    const std::vector<AuraType> m_AuraOrder = { AuraType::First, AuraType::Second };

    NFSEngine::UIObject* m_DialoguePortrait = nullptr;
    NFSEngine::UIObject* m_DialogueName = nullptr;
    NFSEngine::UIObject* m_DialogueNameShadow = nullptr;
    NFSEngine::UIObject* m_DialogueShadow = nullptr;
    NFSEngine::UIObject* m_DialogueMsg = nullptr;

    std::string m_CurrentPortraitPath = "";

    float m_PulseStrengthAura = 0.15f;
    float m_PulseStrengthHearts = 0.10f;
    float m_PulseStrengthScore = 0.10f;
    float m_PulseStrengthCooldown = 0.10f;

    // TUTORIAL
    NFSEngine::UIObject* m_TutorialTexture = nullptr;
    bool m_TutorialShown = false;

    size_t m_TutorialShowEventId = 0;
    size_t m_TutorialHideEventId = 0;

    float m_TutorialTargetAlpha = 0.0f;
    float m_TutorialCurrentAlpha = 0.0f;

    float m_TutorialLerpSpeed = 20;
    std::unordered_map<TutorialPanel, std::shared_ptr<NFSEngine::Texture>> m_TutorialTextures;

protected:
    void OnAwake() override {
        if (m_Canvas == nullptr) {
            m_Canvas = new NFSEngine::Canvas();
        }
    }

    void OnStart() override {
        InitVignetteUI();
        InitScoreUI();
        InitHeartsUI();
        InitAuraUI();
        InitCooldownUI();
        InitDialogueUI();
        InitTutorialUI();

        if (TutorialManager::Instance) {
            m_TutorialShowEventId = TutorialManager::Instance->OnShowTutorial.AddListener([this](TutorialPanel panel) {
                if (m_TutorialTexture && m_TutorialTexture->HasComponent<NFSEngine::ImageComponent>()) {

                    // Podmieniamy teksturę korzystając z tych załadowanych do pamięci
                    if (m_TutorialTextures.find(panel) != m_TutorialTextures.end()) {
                        m_TutorialTexture->GetComponent<NFSEngine::ImageComponent>()->TexturePtr = m_TutorialTextures[panel];
                    }
                }

                this->m_TutorialTargetAlpha = 1.0f;
                this->m_TutorialShown = true;
            });

            m_TutorialHideEventId = TutorialManager::Instance->OnHideTutorial.AddListener([this]() {
                this->m_TutorialTargetAlpha = 0.0f;
                this->m_TutorialShown = false;
            });
        }

        if (AuraManager::Instance) {
            UpdateAuraVisuals(AuraManager::Instance->CurrentAura);
            m_LeftAuraCurrentAlpha = m_LeftAuraTargetAlpha;
            m_LeftAuraCurrentScale = m_LeftAuraTargetScale;
            m_RightAuraCurrentAlpha = m_RightAuraTargetAlpha;
            m_RightAuraCurrentScale = m_RightAuraTargetScale;

            m_TargetVignetteColor
                = (AuraManager::Instance->CurrentAura == AuraType::First) ? m_FirstVignetteColor : m_SecondVignetteColor;
            m_CurrentVignetteColor = m_TargetVignetteColor;

            m_AuraEventId = AuraManager::Instance->OnAuraChanged.AddListener([this](AuraType newAura) {
                this->UpdateAuraVisuals(newAura);

                if (newAura == AuraType::First) {
                    m_TargetVignetteColor = m_FirstVignetteColor;
                } else {
                    m_TargetVignetteColor = m_SecondVignetteColor;
                }
            });
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
        UpdateDialogueUI();

        float lerpSpeed = 5.0f * (float)deltaTime;

        // --- OBLICZANIE PULSU DO RYTMU ---
        float distToBeat = 99999;
        if (AuraManager::Instance->CurrentAura == AuraType::First) {
            distToBeat = NFSEngine::AudioManager::GetDistanceToEventForTrack("Bass");
        } else if (AuraManager::Instance->CurrentAura == AuraType::Second) {
            distToBeat = NFSEngine::AudioManager::GetDistanceToEventForTrack("Piano");
        }

        // Pulse przyjmuje wartość od 0.0 (cisza) do 1.0 (w szczycie uderzenia)
        float peak = 1.0f - (distToBeat * 2.0f);
        float pulse = std::max(0.0f, (peak - 0.5f) * 2.0f);

        // Zmiękczamy puls, by spadek był bardziej naturalny (potęgą)
        float smoothPulse = pow(pulse, 2.0f);

        // --- ANIMACJA WINIETY ---
        m_CurrentVignetteColor += (m_TargetVignetteColor - m_CurrentVignetteColor) * lerpSpeed;
        m_CurrentVignetteAlpha += (m_TargetVignetteAlpha - m_CurrentVignetteAlpha) * lerpSpeed;

        m_CurrentVignetteAlpha = 0.5f + (smoothPulse * 0.5f);
        SetVignetteColor(glm::vec4(m_CurrentVignetteColor, m_CurrentVignetteAlpha));

        float auraLerpSpeed = 7.0f * (float)deltaTime;

        m_LeftAuraCurrentAlpha += (m_LeftAuraTargetAlpha - m_LeftAuraCurrentAlpha) * auraLerpSpeed;
        m_LeftAuraCurrentScale += (m_LeftAuraTargetScale - m_LeftAuraCurrentScale) * auraLerpSpeed;

        m_RightAuraCurrentAlpha += (m_RightAuraTargetAlpha - m_RightAuraCurrentAlpha) * auraLerpSpeed;
        m_RightAuraCurrentScale += (m_RightAuraTargetScale - m_RightAuraCurrentScale) * auraLerpSpeed;

        const float baseAuraSize = 300.0f;

        float leftBonusPulse = (m_LeftAuraTargetScale == 1.0f) ? (smoothPulse * m_PulseStrengthAura) : 0.0f;
        float rightBonusPulse = (m_RightAuraTargetScale == 1.0f) ? (smoothPulse * m_PulseStrengthAura) : 0.0f;

        if (m_LeftAuraImage && m_LeftAuraImage->HasComponent<NFSEngine::ImageComponent>()) {
            m_LeftAuraImage->GetComponent<NFSEngine::ImageComponent>()->Color.a = m_LeftAuraCurrentAlpha;
            m_LeftAuraImage->Transform.Width = baseAuraSize * (m_LeftAuraCurrentScale + leftBonusPulse);
            m_LeftAuraImage->Transform.Height = baseAuraSize * (m_LeftAuraCurrentScale + leftBonusPulse);
        }

        if (m_RightAuraImage && m_RightAuraImage->HasComponent<NFSEngine::ImageComponent>()) {
            m_RightAuraImage->GetComponent<NFSEngine::ImageComponent>()->Color.a = m_RightAuraCurrentAlpha;
            m_RightAuraImage->Transform.Width = baseAuraSize * (m_RightAuraCurrentScale + rightBonusPulse);
            m_RightAuraImage->Transform.Height = baseAuraSize * (m_RightAuraCurrentScale + rightBonusPulse);
        }

        const float baseHeartSize = 150.0f;
        float heartPulseMult = 1.0f + (smoothPulse * m_PulseStrengthHearts);
        for (auto* heart : m_Hearts) {
            if (heart) {
                heart->Transform.Width = baseHeartSize * heartPulseMult;
                heart->Transform.Height = baseHeartSize * heartPulseMult;
            }
        }

        float scorePulseMult = 1.0f + (smoothPulse * m_PulseStrengthScore);

        if (m_TutorialTexture && m_TutorialTexture->HasComponent<NFSEngine::ImageComponent>()) {
            m_TutorialCurrentAlpha += (m_TutorialTargetAlpha - m_TutorialCurrentAlpha) * m_TutorialLerpSpeed * deltaTime;
            m_TutorialTexture->GetComponent<NFSEngine::ImageComponent>()->Color.a = m_TutorialCurrentAlpha;
        }

        for (size_t i = 0; i < m_FullNotes.size(); i++) {
            if (i < m_FullNotesBaseWidths.size()) {

                float baseW = m_FullNotesBaseWidths[i];
                float baseH = m_FullNotesBaseHeights[i];

                float targetW = baseW * scorePulseMult;
                float targetH = baseH * scorePulseMult;

                float widthDiff = targetW - baseW;
                float heightDiff = targetH - baseH;

                float targetWidthTotal = 500.0f;
                float stepX = (targetWidthTotal - baseW) / (m_NumberOfNotes - 1);
                float baseX = ((1920.0f - targetWidthTotal) / 2.0f) + (i * stepX);
                float baseY = 100.0f;

                float targetPosX = baseX - (widthDiff / 2.0f);
                float targetPosY = baseY - (heightDiff / 2.0f);

                if (m_FullNotes[i]) {
                    m_FullNotes[i]->Transform.Width = targetW;
                    m_FullNotes[i]->Transform.Height = targetH;
                    m_FullNotes[i]->Transform.Position.x = targetPosX;
                    m_FullNotes[i]->Transform.Position.y = targetPosY;
                }

                if (i < m_BgNotes.size() && m_BgNotes[i]) {
                    m_BgNotes[i]->Transform.Width = targetW;
                    m_BgNotes[i]->Transform.Height = targetH;
                    m_BgNotes[i]->Transform.Position.x = targetPosX;
                    m_BgNotes[i]->Transform.Position.y = targetPosY;
                }

                if (i < m_EmptyNotes.size() && m_EmptyNotes[i]) {
                    m_EmptyNotes[i]->Transform.Width = targetW;
                    m_EmptyNotes[i]->Transform.Height = targetH;
                    m_EmptyNotes[i]->Transform.Position.x = targetPosX;
                    m_EmptyNotes[i]->Transform.Position.y = targetPosY;
                }
            }
        }

        if (m_CooldownBg && m_CooldownFill && m_CooldownFrame) {
            glm::vec3 darkColor = m_CurrentVignetteColor * 0.25f;
            m_CooldownBg->GetComponent<NFSEngine::ImageComponent>()->Color = glm::vec4(darkColor, 1.0f);
            m_CooldownFill->GetComponent<NFSEngine::ImageComponent>()->Color = glm::vec4(m_CurrentVignetteColor, 1.0f);

            float fillRatio = 1.0f;
            if (AuraManager::Instance) {
                fillRatio = AuraManager::Instance->GetCooldownProgress();
            }

            float cdPulseMult = 1.0f + (smoothPulse * m_PulseStrengthCooldown);

            float baseFrameW = 464.5f;
            float baseFrameH = 46.0f;
            float baseInnerH = 41.0f;

            float currentFrameW = baseFrameW * cdPulseMult;
            float currentFrameH = baseFrameH * cdPulseMult;
            float currentInnerW = m_MaxCooldownWidth * cdPulseMult;
            float currentInnerH = baseInnerH * cdPulseMult;

            m_CooldownFrame->Transform.Width = currentFrameW;
            m_CooldownFrame->Transform.Height = currentFrameH;

            m_CooldownBg->Transform.Width = currentInnerW;
            m_CooldownBg->Transform.Height = currentInnerH;

            m_CooldownFill->Transform.Width = currentInnerW * fillRatio;
            m_CooldownFill->Transform.Height = currentInnerH;

            float centerX = 1920.0f / 2.0f;
            m_CooldownFill->Transform.Position.x = centerX - (currentInnerW / 2.0f);
        }
        // -----------------------------

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
        if (TutorialManager::Instance && m_TutorialShowEventId != 0) {
            TutorialManager::Instance->OnShowTutorial.RemoveListener(m_TutorialShowEventId);
            TutorialManager::Instance->OnHideTutorial.RemoveListener(m_TutorialHideEventId);
            m_TutorialShowEventId = 0;
            m_TutorialHideEventId = 0;
        }
    }

    void SetVignetteColor(const glm::vec4& color) {
        if (m_Vignette && m_Vignette->HasComponent<NFSEngine::ImageComponent>()) {
            m_Vignette->GetComponent<NFSEngine::ImageComponent>()->Color = color;
        }
    }

private:
    void InitTutorialUI() {
        float originalWidth = 400.0f;
        float originalHeight = 600.0f;
        float aspectRatio = originalWidth / originalHeight;

        float targetHeight = 300.0f;
        float targetWidth = targetHeight * aspectRatio;

        NFSEngine::UI::ImageParameters tutParams;
        tutParams.position = glm::vec3(1920.0f / 2.0f, (NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f) - 250.0f, 50.0f);
        tutParams.width = targetWidth;
        tutParams.height = targetHeight;
        tutParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

        m_TutorialTexture = &NFSEngine::UI::Image(*m_Canvas, tutParams);
        m_TutorialTexture->Transform.Pivot = glm::vec2(0.5f, 0.5f);

        // --- PRELOADING TEKSTUR ---
        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;

        m_TutorialTextures[TutorialPanel::Walk]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_ruch.png", texParam);
        m_TutorialTextures[TutorialPanel::LookAround]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_patrzenie.png", texParam);
        m_TutorialTextures[TutorialPanel::Jump]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_jump.png", texParam);
        m_TutorialTextures[TutorialPanel::Checkpoint]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_checkpoint.png", texParam);
        m_TutorialTextures[TutorialPanel::Pause]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_pause.png", texParam);
        m_TutorialTextures[TutorialPanel::WallJump]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_walljump.png", texParam);
        m_TutorialTextures[TutorialPanel::Aura]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_aura.png", texParam);
        m_TutorialTextures[TutorialPanel::DoubleJump]
            = NFSEngine::Texture::Create("assets/textures/ui/tutorial/tutorial_doublejump.png", texParam);
    }

    std::string GetTutorialTexturePath(TutorialPanel panel) {
        switch (panel) {
        case TutorialPanel::Walk:
            return "assets/textures/ui/tutorial/tutorial_ruch.png";
        case TutorialPanel::LookAround:
            return "assets/textures/ui/tutorial/tutorial_patrzenie.png";
        case TutorialPanel::Jump:
            return "assets/textures/ui/tutorial/tutorial_jump.png";
        case TutorialPanel::Checkpoint:
            return "assets/textures/ui/tutorial/tutorial_checkpoint.png";
        case TutorialPanel::Pause:
            return "assets/textures/ui/tutorial/tutorial_pause.png";
        case TutorialPanel::WallJump:
            return "assets/textures/ui/tutorial/tutorial_walljump.png";
        case TutorialPanel::Aura:
            return "assets/textures/ui/tutorial/tutorial_aura.png";
        case TutorialPanel::DoubleJump:
            return "assets/textures/ui/tutorial/tutorial_doublejump.png";
        default:
            return "";
        }
    }

    void InitVignetteUI() { /* Bez zmian */
        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;
        texParam.Channels = 4;
        texParam.sRGB = false;

        auto vignetteTex = NFSEngine::Texture::Create("assets/textures/ui/hud/vignette.png", texParam);

        NFSEngine::UI::ImageParameters vignetteParams;
        vignetteParams.position = glm::vec3(960.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f, 0.1f);
        vignetteParams.width = 1920.0f;
        vignetteParams.height = NFSEngine::UIRenderer::VIRTUAL_HEIGHT;
        vignetteParams.color = glm::vec4(m_CurrentVignetteColor, m_CurrentVignetteAlpha);
        vignetteParams.texture = vignetteTex;

        m_Vignette = &NFSEngine::UI::Image(*m_Canvas, vignetteParams);
    }

    void InitScoreUI() {
        float originalWidth = 462;
        float originalHeight = 591;
        float aspectRatio = originalWidth / originalHeight;

        float noteHeight = 100;
        float noteWidth = noteHeight * aspectRatio;
        float targetWidth = 500;

        glm::vec3 firstPosition = { (1920 - targetWidth) / 2.0f, 100.0f, 1.0f };
        float stepX = (targetWidth - noteWidth) / (m_NumberOfNotes - 1);

        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;
        auto bgTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_background.png", texParam);
        auto emptyTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_empty_v2.png", texParam);
        auto fullTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_full.png", texParam);
        auto fullExtraTex = NFSEngine::Texture::Create("assets/textures/ui/notes/note_ui_full_extra.png", texParam);

        // Rezerwacja pamięci
        m_BgNotes.reserve(m_NumberOfNotes);
        m_EmptyNotes.reserve(m_NumberOfNotes);
        m_FullNotes.reserve(m_NumberOfNotes);
        m_FullNotesBaseWidths.reserve(m_NumberOfNotes);
        m_FullNotesBaseHeights.reserve(m_NumberOfNotes);

        // --- RYSOWANIE TEŁ ---
        for (int i = 0; i < m_NumberOfNotes; ++i) {
            NFSEngine::UI::ImageParameters noteParameters;
            glm::vec3 currentPosition = firstPosition;
            currentPosition.x += i * stepX;
            noteParameters.position = currentPosition;
            noteParameters.width = noteWidth;
            noteParameters.height = noteHeight;
            noteParameters.color = glm::vec4(1, 1, 1, 1.0f);
            noteParameters.texture = bgTex;

            auto* bgObj = &NFSEngine::UI::Image(*m_Canvas, noteParameters);
            bgObj->Transform.Pivot = glm::vec2(0.5f, 0.5f);
            bgObj->Transform.Position.x += (noteWidth / 2.0f);
            bgObj->Transform.Position.y += (noteHeight / 2.0f);
            m_BgNotes.push_back(bgObj);
        }

        // --- RYSOWANIE PUSTYCH NUTEK ---
        for (int i = 0; i < m_NumberOfNotes; ++i) {
            NFSEngine::UI::ImageParameters noteParameters;
            glm::vec3 currentPosition = firstPosition;
            currentPosition.x += i * stepX;
            noteParameters.position = currentPosition + glm::vec3(0, 0, 1 + (1 * i));
            noteParameters.width = noteWidth;
            noteParameters.height = noteHeight;
            noteParameters.color = glm::vec4(1, 1, 1, 1.0f);
            noteParameters.texture = emptyTex;

            auto* emptyObj = &NFSEngine::UI::Image(*m_Canvas, noteParameters);
            emptyObj->Transform.Pivot = glm::vec2(0.5f, 0.5f);
            emptyObj->Transform.Position.x += (noteWidth / 2.0f);
            emptyObj->Transform.Position.y += (noteHeight / 2.0f);
            m_EmptyNotes.push_back(emptyObj);
        }

        // --- RYSOWANIE WYPEŁNIEŃ ---
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

            auto* noteObj = &NFSEngine::UI::Image(*m_Canvas, noteParameters);
            noteObj->Transform.Pivot = glm::vec2(0.5f, 0.5f);
            noteObj->Transform.Position.x += (noteWidth / 2.0f);
            noteObj->Transform.Position.y += (noteHeight / 2.0f);

            m_FullNotes.push_back(noteObj);
            m_FullNotesBaseWidths.push_back(noteWidth);
            m_FullNotesBaseHeights.push_back(noteHeight);
        }
    }

    void InitHeartsUI() {
        constexpr int k_MaxLives = 3;

        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;

        m_HeartFullTex = NFSEngine::Texture::Create("assets/textures/ui/hud/heart.png", texParam);
        m_HeartEmptyTex = NFSEngine::Texture::Create("assets/textures/ui/hud/heart_empty.png", texParam);

        const float heartSize = 150.0f;
        const float heartSpacing = 120.0f;
        const float heartStartX = 10.0f;
        const float heartY = 20.0f;

        m_Hearts.reserve(k_MaxLives);
        for (int i = 0; i < k_MaxLives; ++i) {
            NFSEngine::UI::ImageParameters heartParams;
            heartParams.position = glm::vec3(heartStartX + i * heartSpacing, heartY, 1.0f);
            heartParams.width = heartSize;
            heartParams.height = heartSize;

            heartParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            heartParams.texture = m_HeartFullTex;

            auto* heartObj = &NFSEngine::UI::Image(*m_Canvas, heartParams);

            // Ustawiamy Pivot na środek grafiki, żeby serca pompowały równomiernie dookoła osi, a nie w prawo-dół
            heartObj->Transform.Pivot = glm::vec2(0.5f, 0.5f);
            heartObj->Transform.Position.x += (heartSize / 2.0f);
            heartObj->Transform.Position.y += (heartSize / 2.0f);

            m_Hearts.push_back(heartObj);
        }
    }

    void InitAuraUI() { /* Bez zmian */
        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;
        texParam.Channels = 4;
        texParam.sRGB = false;

        auto doubleJumpTex = NFSEngine::Texture::Create("assets/textures/ui/hud/doublejump_aura.png", texParam);
        auto dashTex = NFSEngine::Texture::Create("assets/textures/ui/hud/dash_aura.png", texParam);

        float screenBottomY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT;
        float xOffset = 85.0f;
        float yOffset = 65.0f;

        NFSEngine::UI::ImageParameters leftParams;
        leftParams.position = glm::vec3(-xOffset, screenBottomY + yOffset, 1.0f);
        leftParams.width = 300.0f;
        leftParams.height = 300.0f;
        leftParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        leftParams.texture = doubleJumpTex;
        m_LeftAuraImage = &NFSEngine::UI::Image(*m_Canvas, leftParams);
        m_LeftAuraImage->Transform.Pivot = glm::vec2(0.0f, 1.0f);

        NFSEngine::UI::ImageParameters rightParams;
        rightParams.position = glm::vec3(1920.0f + xOffset, screenBottomY + yOffset, 1.0f);
        rightParams.width = 300.0f;
        rightParams.height = 300.0f;
        rightParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        rightParams.texture = dashTex;
        m_RightAuraImage = &NFSEngine::UI::Image(*m_Canvas, rightParams);
        m_RightAuraImage->Transform.Pivot = glm::vec2(1.0f, 1.0f);
    }

    void InitCooldownUI() { /* Bez zmian */
        float frameWidth = 464.5f;
        float frameHeight = 46.0f;

        float innerWidth = m_MaxCooldownWidth;
        float innerHeight = 41.0f;

        float centerX = 1920.0f / 2.0f;
        float bottomY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 30.0f;

        NFSEngine::UI::ImageParameters bgParams;
        bgParams.position = glm::vec3(centerX, bottomY, 1.5f);
        bgParams.width = innerWidth;
        bgParams.height = innerHeight;
        bgParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_CooldownBg = &NFSEngine::UI::Image(*m_Canvas, bgParams);
        m_CooldownBg->Transform.Pivot = glm::vec2(0.5f, 0.5f);

        NFSEngine::UI::ImageParameters fillParams;
        fillParams.position = glm::vec3(centerX - (innerWidth / 2.0f), bottomY, 1.6f);
        fillParams.width = innerWidth;
        fillParams.height = innerHeight;
        fillParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_CooldownFill = &NFSEngine::UI::Image(*m_Canvas, fillParams);
        m_CooldownFill->Transform.Pivot = glm::vec2(0.0f, 0.5f);

        NFSEngine::TextureParameters texParam;
        texParam.WrapS = NFSEngine::TextureWrap::Clamp;
        texParam.WrapT = NFSEngine::TextureWrap::Clamp;
        auto frameTex = NFSEngine::Texture::Create("assets/textures/ui/hud/aura_cd_frame.png", texParam);

        NFSEngine::UI::ImageParameters frameParams;
        frameParams.position = glm::vec3(centerX, bottomY, 1.7f);
        frameParams.width = frameWidth;
        frameParams.height = frameHeight;
        frameParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        frameParams.texture = frameTex;
        m_CooldownFrame = &NFSEngine::UI::Image(*m_Canvas, frameParams);
        m_CooldownFrame->Transform.Pivot = glm::vec2(0.5f, 0.5f);
    }

    void UpdateHeartVisuals(int currentLives) { /* Bez zmian */
        for (size_t i = 0; i < m_Hearts.size(); ++i) {
            if (!m_Hearts[i]) continue;

            auto* img = m_Hearts[i]->GetComponent<NFSEngine::ImageComponent>();
            if (!img) continue;

            img->Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            if (static_cast<int>(i) < currentLives) {
                img->TexturePtr = m_HeartFullTex;
            } else {
                img->TexturePtr = m_HeartEmptyTex;
            }
        }
    }

    void UpdateAuraVisuals(AuraType newAura) { /* Bez zmian */
        float activeAlpha = 1.0f;
        float inactiveAlpha = 0.4f;

        float activeScale = 1.0f;
        float inactiveScale = 0.8f;

        if (newAura == AuraType::First) {
            m_LeftAuraTargetAlpha = activeAlpha;
            m_LeftAuraTargetScale = activeScale;

            m_RightAuraTargetAlpha = inactiveAlpha;
            m_RightAuraTargetScale = inactiveScale;
        } else {
            m_LeftAuraTargetAlpha = inactiveAlpha;
            m_LeftAuraTargetScale = inactiveScale;

            m_RightAuraTargetAlpha = activeAlpha;
            m_RightAuraTargetScale = activeScale;
        }
    }

    void InitDialogueUI() { /* Bez zmian */
        const float bottomY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 100.0f;
        const float textStartX = 650.0f;

        NFSEngine::UI::ImageParameters portraitParams;
        portraitParams.position = glm::vec3(650.0f - 200.0f, NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 150.0f, 10.0f);
        portraitParams.width = 640.0f;
        portraitParams.height = 360.0f;
        portraitParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_DialoguePortrait = &NFSEngine::UI::Image(*m_Canvas, portraitParams);
        m_DialoguePortrait->Transform.Pivot = glm::vec2(0.5f, 0.5f);

        NFSEngine::UI::LabelParameters nameShadowParams;
        nameShadowParams.position = glm::vec3(textStartX + 3.0f, bottomY - 137.0f, 10.1f);
        nameShadowParams.text = "";
        nameShadowParams.font = NFSEngine::DialogueManager::Get().GetFont();
        nameShadowParams.scale = 1.2f;
        nameShadowParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        m_DialogueNameShadow = &NFSEngine::UI::Label(*m_Canvas, nameShadowParams);
        m_DialogueNameShadow->Transform.Pivot.x = 0.0f;
        m_DialogueNameShadow->Transform.Pivot.y = 0.0f;

        NFSEngine::UI::LabelParameters nameParams;
        nameParams.position = glm::vec3(textStartX, bottomY - 140.0f, 10.1f);
        nameParams.text = "";
        nameParams.font = NFSEngine::DialogueManager::Get().GetFont();
        nameParams.scale = 1.2f;
        nameParams.color = glm::vec4(0.8f, 0.8f, 0.2f, 0.0f);
        m_DialogueName = &NFSEngine::UI::Label(*m_Canvas, nameParams);
        m_DialogueName->Transform.Pivot.x = 0.0f;
        m_DialogueName->Transform.Pivot.y = 0.0f;

        NFSEngine::UI::LabelParameters shadowParams;
        shadowParams.position = glm::vec3(textStartX + 3.0f, bottomY - 62.0f, 10.0f);
        shadowParams.text = "";
        shadowParams.scale = 1.8f;
        shadowParams.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        shadowParams.font = NFSEngine::DialogueManager::Get().GetFont();
        m_DialogueShadow = &NFSEngine::UI::Label(*m_Canvas, shadowParams);
        m_DialogueShadow->Transform.Pivot.x = 0.0f;
        m_DialogueShadow->Transform.Pivot.y = 0.0f;

        NFSEngine::UI::LabelParameters msgParams;
        msgParams.position = glm::vec3(textStartX, bottomY - 65.0f, 10.1f);
        msgParams.text = "";
        msgParams.scale = 1.8f;
        msgParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        msgParams.font = NFSEngine::DialogueManager::Get().GetFont();
        m_DialogueMsg = &NFSEngine::UI::Label(*m_Canvas, msgParams);
        m_DialogueMsg->Transform.Pivot.x = 0.0f;
        m_DialogueMsg->Transform.Pivot.y = 0.0f;
    }

    void UpdateScoreText(int newScore) {
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

    void UpdateDialogueUI() { /* Bez zmian */
        const auto& dialogue = NFSEngine::DialogueManager::Get().GetActiveDialogue();

        auto SetLabelState = [](NFSEngine::UIObject* obj, const std::string& text, float alpha) {
            if (obj && obj->HasComponent<NFSEngine::TextComponent>()) {
                auto* tc = obj->GetComponent<NFSEngine::TextComponent>();
                tc->TextString = text;
                tc->Color.a = alpha;
            }
        };

        if (dialogue.IsVisible) {
            SetLabelState(m_DialogueName, dialogue.SpeakerName + ":", 1.0f);
            SetLabelState(m_DialogueNameShadow, dialogue.SpeakerName + ":", 1.0f);
            SetLabelState(m_DialogueShadow, dialogue.DisplayedMessage, 1.0f);
            SetLabelState(m_DialogueMsg, dialogue.DisplayedMessage, 1.0f);

            if (m_DialoguePortrait && m_DialoguePortrait->HasComponent<NFSEngine::ImageComponent>()) {
                auto* imgComp = m_DialoguePortrait->GetComponent<NFSEngine::ImageComponent>();
                imgComp->Color.a = 1.0f;

                if (m_CurrentPortraitPath != dialogue.PortraitPath && !dialogue.PortraitPath.empty()) {

                    NFSEngine::TextureParameters portraitParams;
                    portraitParams.WrapS = NFSEngine::TextureWrap::Clamp;
                    portraitParams.WrapT = NFSEngine::TextureWrap::Clamp;
                    portraitParams.sRGB = false;

                    imgComp->TexturePtr = NFSEngine::Texture::Create(dialogue.PortraitPath, portraitParams);
                    m_CurrentPortraitPath = dialogue.PortraitPath;
                }
            }
        } else {
            SetLabelState(m_DialogueName, "", 0.0f);
            SetLabelState(m_DialogueNameShadow, "", 0.0f);
            SetLabelState(m_DialogueShadow, "", 0.0f);
            SetLabelState(m_DialogueMsg, "", 0.0f);

            if (m_DialoguePortrait && m_DialoguePortrait->HasComponent<NFSEngine::ImageComponent>()) {
                m_DialoguePortrait->GetComponent<NFSEngine::ImageComponent>()->Color.a = 0.0f;
            }
        }
    }
};