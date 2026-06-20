#include "Layers/OptionsLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"
#include "Core/Input.hpp"
#include <cmath>

OptionsLayer::OptionsLayer() { m_Canvas = nullptr; }

OptionsLayer::~OptionsLayer() {
    if (m_Canvas) delete m_Canvas;
}

void OptionsLayer::OnAttach() {
    m_Canvas = new NFSEngine::Canvas();
    SyncResolutionIndex();
    BuildUI();
}

void OptionsLayer::OnDetach() { }

void OptionsLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_Canvas) m_Canvas->Update();

    auto& input = NFSEngine::InputActionManager::Get();
    bool uiChanged = false;

    // --- Zmiana wiersza ---
    if (input.IsDown("UINavDown")) {
        m_FocusedRow = (m_FocusedRow + 1) % 5;
        uiChanged = true;
    } else if (input.IsDown("UINavUp")) {
        m_FocusedRow = (m_FocusedRow - 1 + 5) % 5;
        uiChanged = true;
    }

    // --- Lewo / Prawo (dla opcji z suwakami) ---
    if (input.IsDown("UINavLeft")) {
        if (m_FocusedRow == 1) {
            float vol = NFSEngine::AudioEngine::GetMasterVolume();
            if (vol > 0.0f) {
                NFSEngine::AudioEngine::SetMasterVolume(vol - 0.1f);
                uiChanged = true;
            }
        }
        if (m_FocusedRow == 3) {
            if (m_CurrentResIndex > 0) {
                m_CurrentResIndex--;
                auto [w, h] = m_Resolutions[m_CurrentResIndex];
                NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
                uiChanged = true;
            }
        }
    }
    if (input.IsDown("UINavRight")) {
        if (m_FocusedRow == 1) {
            float vol = NFSEngine::AudioEngine::GetMasterVolume();
            if (vol < 1.0f) {
                NFSEngine::AudioEngine::SetMasterVolume(vol + 0.1f);
                uiChanged = true;
            }
        }
        if (m_FocusedRow == 3) {
            if (m_CurrentResIndex < m_Resolutions.size() - 1) {
                m_CurrentResIndex++;
                auto [w, h] = m_Resolutions[m_CurrentResIndex];
                NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
                uiChanged = true;
            }
        }
    }

    // --- Zatwierdzenie (Tylko dla guzików On/Off/Back) ---
    if (input.IsDown("UIConfirm")) {
        if (m_FocusedRow == 0) {
            bool isVsync = NFSEngine::Application::Get().GetWindow().IsVSync();
            NFSEngine::Application::Get().GetWindow().SetVSync(!isVsync);
            uiChanged = true;
        }
        if (m_FocusedRow == 2) {
            bool isFullscreen = NFSEngine::Application::Get().GetWindow().IsFullscreen();
            NFSEngine::Application::Get().GetWindow().SetFullscreen(!isFullscreen);
            if (isFullscreen) {
                NFSEngine::Application::Get().GetWindow().SetWindowSize(1600, 900);
                m_CurrentResIndex = 1;
            } else {
                SyncResolutionIndex();
            }
            uiChanged = true;
        }
        if (m_FocusedRow == 4) {
            GameManager::Get().CloseOptions();
        }
    }

    if (uiChanged) BuildUI();
}

void OptionsLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_Canvas) {
        m_Canvas->Draw();
    }
    NFSEngine::UIRenderer::End();
}

void OptionsLayer::OnEvent(NFSEngine::Event& e) {
    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;

        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().CloseOptions();
            e.Handled = true;
        }
    }
}

void OptionsLayer::BuildUI() {
    m_Canvas->ClearUIObjects();

    const float centerX = NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f;
    const float centerY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f;

    const float leftStart = centerX - 350.0f;
    const float rightStart = centerX + 350.0f;

    glm::vec4 normalColor(0.4f, 0.4f, 0.4f, 1.0f);
    glm::vec4 focusColor(0.8f, 0.8f, 0.2f, 1.0f);

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(centerX, centerY, 0.1f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH - 20.0f;
    bgParams.height = NFSEngine::UIRenderer::VIRTUAL_HEIGHT - 20.0f;
    bgParams.color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    float currentY = 150.0f;
    float elementSpacing = 120.0f;
    float buttonHeight = 75.0f;

    // Title
    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(centerX, currentY, 0.5f);
    titleParams.text = "OPTIONS";
    titleParams.scale = 2.5f;
    titleParams.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    // Title background
    NFSEngine::UI::ImageParameters titleBgParams;
    titleBgParams.position = glm::vec3(centerX, currentY, 0.4f);
    titleBgParams.width = 800.0f;
    titleBgParams.height = 120.0f;
    titleBgParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    NFSEngine::UI::Image(*m_Canvas, titleBgParams);

    // Start pierwszego rzędu opcji (V-SYNC)
    currentY += elementSpacing + 50.0f;

    NFSEngine::UI::LabelParameters vsyncLabel;
    vsyncLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    vsyncLabel.text = "V-SYNC";
    vsyncLabel.scale = 0.8f;
    vsyncLabel.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, vsyncLabel);

    bool isVsync = NFSEngine::Application::Get().GetWindow().IsVSync();

    NFSEngine::UI::ButtonParameters vsyncBtn;
    vsyncBtn.position = glm::vec3(rightStart, currentY, 0.5f);
    vsyncBtn.width = 300.0f;
    vsyncBtn.height = buttonHeight;
    vsyncBtn.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    vsyncBtn.text = isVsync ? "ON" : "OFF";
    vsyncBtn.onClick = [this, isVsync]() {
        NFSEngine::Application::Get().GetWindow().SetVSync(!isVsync);
        this->BuildUI();
    };
    vsyncBtn.color = (m_FocusedRow == 0) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, vsyncBtn);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters volLabel;
    volLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    volLabel.text = "VOLUME";
    volLabel.scale = 0.8f;
    volLabel.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, volLabel);

    NFSEngine::UI::ButtonParameters volMinus;
    volMinus.position = glm::vec3(rightStart - 120.0f, currentY, 0.5f);
    volMinus.width = buttonHeight;
    volMinus.height = buttonHeight;
    volMinus.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    volMinus.text = "<";
    volMinus.onClick = [this]() {
        float vol = NFSEngine::AudioEngine::GetMasterVolume();
        if (vol > 0.0f) NFSEngine::AudioEngine::SetMasterVolume(vol - 0.1f);
        this->BuildUI();
    };
    volMinus.color = (m_FocusedRow == 1) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, volMinus);

    float currentVol = NFSEngine::AudioEngine::GetMasterVolume();

    NFSEngine::UI::LabelParameters volValue;
    volValue.position = glm::vec3(rightStart, currentY, 0.5f);
    volValue.text = std::to_string(static_cast<int>(std::round(currentVol * 100.0f)));
    volValue.scale = 0.8f;
    volValue.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, volValue);

    NFSEngine::UI::ButtonParameters volPlus;
    volPlus.position = glm::vec3(rightStart + 120.0f, currentY, 0.5f);
    volPlus.width = buttonHeight;
    volPlus.height = buttonHeight;
    volPlus.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    volPlus.text = ">";
    volPlus.onClick = [this]() {
        float vol = NFSEngine::AudioEngine::GetMasterVolume();
        if (vol < 1.0f) NFSEngine::AudioEngine::SetMasterVolume(vol + 0.1f);
        this->BuildUI();
    };
    volPlus.color = (m_FocusedRow == 1) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, volPlus);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters fsLabel;
    fsLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    fsLabel.text = "FULLSCREEN";
    fsLabel.scale = 0.8f;
    fsLabel.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, fsLabel);

    bool isFullscreen = NFSEngine::Application::Get().GetWindow().IsFullscreen();

    NFSEngine::UI::ButtonParameters fsBtn;
    fsBtn.position = glm::vec3(rightStart, currentY, 0.5f);
    fsBtn.width = 300.0f;
    fsBtn.height = buttonHeight;
    fsBtn.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    fsBtn.text = isFullscreen ? "ON" : "WINDOWED";

    fsBtn.onClick = [this, isFullscreen]() {
        NFSEngine::Application::Get().GetWindow().SetFullscreen(!isFullscreen);

        if (isFullscreen) {
            NFSEngine::Application::Get().GetWindow().SetWindowSize(1600, 900);
            m_CurrentResIndex = 1;
        } else {
            this->SyncResolutionIndex();
        }

        this->BuildUI();
    };
    fsBtn.color = (m_FocusedRow == 2) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, fsBtn);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters resLabel;
    resLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    resLabel.text = "RESOLUTION";
    resLabel.scale = 0.8f;
    resLabel.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, resLabel);

    NFSEngine::UI::ButtonParameters resMinus;
    resMinus.position = glm::vec3(rightStart - 170.0f, currentY, 0.5f);
    resMinus.width = buttonHeight;
    resMinus.height = buttonHeight;
    resMinus.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    resMinus.text = "<";
    resMinus.onClick = [this]() {
        if (m_CurrentResIndex > 0) {
            m_CurrentResIndex--;
            auto [w, h] = m_Resolutions[m_CurrentResIndex];
            NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
            this->BuildUI();
        }
    };
    resMinus.color = (m_FocusedRow == 3) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, resMinus);

    NFSEngine::UI::LabelParameters resValue;
    resValue.position = glm::vec3(rightStart, currentY, 0.5f);
    std::string resString
        = std::to_string(m_Resolutions[m_CurrentResIndex].first) + "x" + std::to_string(m_Resolutions[m_CurrentResIndex].second);
    resValue.text = resString;
    resValue.scale = 0.8f;
    resValue.color = glm::vec4(1.0f);
    NFSEngine::UI::Label(*m_Canvas, resValue);

    NFSEngine::UI::ButtonParameters resPlus;
    resPlus.position = glm::vec3(rightStart + 170.0f, currentY, 0.5f);
    resPlus.width = buttonHeight;
    resPlus.height = buttonHeight;
    resPlus.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    resPlus.text = ">";
    resPlus.onClick = [this]() {
        if (m_CurrentResIndex < m_Resolutions.size() - 1) {
            m_CurrentResIndex++;
            auto [w, h] = m_Resolutions[m_CurrentResIndex];
            NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
            this->BuildUI();
        }
    };
    resPlus.color = (m_FocusedRow == 3) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, resPlus);

    currentY += elementSpacing + 50.0f;

    NFSEngine::UI::ButtonParameters backParams;
    backParams.position = glm::vec3(centerX, currentY, 0.5f);
    backParams.width = 400.0f;
    backParams.height = buttonHeight;
    backParams.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    backParams.text = "BACK";
    backParams.onClick = []() { GameManager::Get().CloseOptions(); };
    backParams.color = (m_FocusedRow == 4) ? focusColor : normalColor;
    NFSEngine::UI::Button(*m_Canvas, backParams);
}

void OptionsLayer::SyncResolutionIndex() {
    uint32_t currentW = NFSEngine::Application::Get().GetWindow().GetWidth();
    uint32_t currentH = NFSEngine::Application::Get().GetWindow().GetHeight();

    for (size_t i = 0; i < m_Resolutions.size(); ++i) {
        if (m_Resolutions[i].first == currentW && m_Resolutions[i].second == currentH) {
            m_CurrentResIndex = (int)i;
            return;
        }
    }

    int closestIndex = 0;
    int minDiff = 999999;

    for (size_t i = 0; i < m_Resolutions.size(); ++i) {
        int diff = std::abs((int)m_Resolutions[i].first - (int)currentW);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = (int)i;
        }
    }

    m_CurrentResIndex = closestIndex;
}