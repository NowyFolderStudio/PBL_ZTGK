#pragma once

#include <NFSEngine.h>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <algorithm>

#include "Components/Component.hpp"
#include "UI/UIComponents.hpp"
#include "UI/UIFactory.hpp"

class LoadingScreenComponent : public NFSEngine::Component {
public:
    LoadingScreenComponent(NFSEngine::GameObject* owner)
        : NFSEngine::Component(owner) {
        m_Canvas = new NFSEngine::Canvas();
        InitUI();
    }

    ~LoadingScreenComponent() override {
        if (m_Canvas) delete m_Canvas;
    }

    std::string GetName() const override { return "LoadingScreenComponent"; }

    void RenderUI() {
        if (m_Canvas && m_IsVisible) {
            NFSEngine::UIRenderer::Begin();
            m_Canvas->Draw();
            NFSEngine::UIRenderer::End();
        }
    }

    void SetVisible(bool visible) { m_IsVisible = visible; }

    void UpdateProgress(float progress) {
        // 1. Zmiana szerokości - używamy bezpośrednio pola Transform!
        if (m_ProgressBarFill) {
            m_ProgressBarFill->Transform.Width = m_MaxBarWidth * progress;
        }

        // 2. Aktualizacja tekstu (tutaj HasComponent jest prawidłowe, bo to osobny dodawany komponent)
        if (m_ProgressText && m_ProgressText->HasComponent<NFSEngine::TextComponent>()) {
            auto* textComp = m_ProgressText->GetComponent<NFSEngine::TextComponent>();
            int percent = std::min(static_cast<int>(progress * 100.0f), 100);
            textComp->TextString = "LOADING... " + std::to_string(percent) + "%";
        }
    }

protected:
    void OnAwake() override {
        if (m_Canvas == nullptr) {
            m_Canvas = new NFSEngine::Canvas();
        }
        if (!m_UIInitialized) {
            InitUI();
        }
    }

    void OnStart() override { }

    void OnUpdate(NFSEngine::DeltaTime deltaTime) override {
        if (m_Canvas && m_IsVisible) {
            m_Canvas->Update();
        }
    }

private:
    void InitUI() {
        float screenWidth = 1920.0f;
        float screenHeight = 1080.0f;

        m_MaxBarWidth = 800.0f;
        float barHeight = 40.0f;

        float centerX = screenWidth / 2.0f;
        float barY = 500.0f;

        // 1. Główne tło
        NFSEngine::UI::ImageParameters bgParams;
        bgParams.position = glm::vec3(centerX, screenHeight / 2.0f, 0.1f);
        bgParams.width = 4000.0f;
        bgParams.height = 4000.0f;
        bgParams.color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
        NFSEngine::UI::Image(*m_Canvas, bgParams);

        // 2. Tło paska (wycentrowane)
        NFSEngine::UI::ImageParameters barBgParams;
        barBgParams.position = glm::vec3(centerX, barY, 0.2f);
        barBgParams.width = m_MaxBarWidth;
        barBgParams.height = barHeight;
        barBgParams.color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        NFSEngine::UI::Image(*m_Canvas, barBgParams);

        // 3. Wypełnienie paska (startuje jako wyzerowane)
        float leftEdgeX = centerX - (m_MaxBarWidth / 2.0f);

        NFSEngine::UI::ImageParameters barFillParams;
        barFillParams.position = glm::vec3(leftEdgeX, barY, 0.3f);
        barFillParams.width = 0.0f; // Bezpieczne 0, zadziała dzięki prawidłowemu Pivotowi
        barFillParams.height = barHeight;
        barFillParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_ProgressBarFill = &NFSEngine::UI::Image(*m_Canvas, barFillParams);

        // Ustawienie pivota bezpośrednio na zmiennej Transform - punkt zerowy na osi X
        m_ProgressBarFill->Transform.Pivot.x = 0.0f;

        // 4. Tekst informacyjny
        NFSEngine::UI::LabelParameters textParams;
        textParams.position = glm::vec3(centerX, barY + 60.0f, 0.4f);
        textParams.text = "LOADING... 0%";
        textParams.scale = 1.0f;
        textParams.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        m_ProgressText = &NFSEngine::UI::Label(*m_Canvas, textParams);

        m_UIInitialized = true;
    }

    bool m_UIInitialized = false;
    NFSEngine::Canvas* m_Canvas = nullptr;
    bool m_IsVisible = true;
    float m_MaxBarWidth = 800.0f;

    NFSEngine::UIObject* m_ProgressBarFill = nullptr;
    NFSEngine::UIObject* m_ProgressText = nullptr;
};