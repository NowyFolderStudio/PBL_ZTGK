#include "Layers/OptionsLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"
#include "Core/Input.hpp"
#include "Core/Log.hpp"
#include <cmath>

OptionsLayer::OptionsLayer() {
    m_Canvas = nullptr;
    m_BackgroundCanvas = nullptr;
}

OptionsLayer::~OptionsLayer() {
    delete m_Canvas;
    delete m_BackgroundCanvas;
}

void OptionsLayer::OnAttach() {
    m_Canvas = new NFSEngine::Canvas();
    m_BackgroundCanvas = new NFSEngine::Canvas();

    m_Font = std::make_shared<NFSEngine::Text>("assets/fonts/Super-Pandora.ttf", 72);

    for (int i = 0; i < 5; ++i) {
        m_RowScales[i] = (i == m_FocusedRow) ? 1.1f : 0.8f;
    }
    auto& bgVideoObj = m_BackgroundCanvas->CreateUIObject();
    const float centerX = NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f;
    const float centerY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f;
    bgVideoObj.Transform.Position = glm::vec3(centerX, centerY, 0.1f);
    bgVideoObj.Transform.Width = NFSEngine::UIRenderer::VIRTUAL_WIDTH;
    bgVideoObj.Transform.Height = -NFSEngine::UIRenderer::VIRTUAL_HEIGHT;

    auto& videoComp = bgVideoObj.AddComponent<NFSEngine::VideoComponent>();

    if (!videoComp.OpenFile("assets/videos/bg_lq.mpg")) {
        videoComp.Color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    }

    SyncResolutionIndex();
    BuildUI();
}

void OptionsLayer::OnDetach() { }

void OptionsLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (m_BackgroundCanvas) m_BackgroundCanvas->Update(deltaTime);
    if (m_Canvas) m_Canvas->Update(deltaTime);

    auto& input = NFSEngine::InputActionManager::Get();
    bool uiChanged = false;

    if (input.IsDown("UINavDown")) {
        m_FocusedRow = (m_FocusedRow + 1) % 5;
        uiChanged = true;
    } else if (input.IsDown("UINavUp")) {
        m_FocusedRow = (m_FocusedRow - 1 + 5) % 5;
        uiChanged = true;
    }

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

    float dt = deltaTime.GetSeconds();

    float animationSpeed = 15.0f;
    for (int i = 0; i < 5; ++i) {
        float targetScale = (m_FocusedRow == i) ? 1.1f : 0.8f;
        m_RowScales[i] += (targetScale - m_RowScales[i]) * animationSpeed * dt;

        if (m_RowLabelTexts[i]) m_RowLabelTexts[i]->Scale = m_RowScales[i];
        if (m_RowLabelShadows[i]) m_RowLabelShadows[i]->Scale = m_RowScales[i];
        if (m_RowValueTexts[i]) m_RowValueTexts[i]->Scale = m_RowScales[i];
        if (m_RowValueShadows[i]) m_RowValueShadows[i]->Scale = m_RowScales[i];

        if (m_RowMinusTexts[i]) m_RowMinusTexts[i]->Scale = m_RowScales[i];
        if (m_RowMinusShadows[i]) m_RowMinusShadows[i]->Scale = m_RowScales[i];
        if (m_RowPlusTexts[i]) m_RowPlusTexts[i]->Scale = m_RowScales[i];
        if (m_RowPlusShadows[i]) m_RowPlusShadows[i]->Scale = m_RowScales[i];
    }

    if (m_PierdolniczekTransform) {
        float indicatorSpeed = 15.0f;
        float currentY = m_PierdolniczekTransform->Position.y;
        float targetY = m_PierdolniczekPosition[m_FocusedRow].y;
        float currentX = m_PierdolniczekTransform->Position.x;
        float targetX = m_PierdolniczekPosition[m_FocusedRow].x;

        float newY = currentY + (targetY - currentY) * indicatorSpeed * dt;
        float newX = currentX + (targetX - currentX) * indicatorSpeed * dt;

        m_PierdolniczekTransform->Position.y = newY;
        m_PierdolniczekTransform->Position.x = newX;
    }

    if (uiChanged || m_NeedsRebuild) {
        BuildUI();
        m_NeedsRebuild = false;
    }
}

void OptionsLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_BackgroundCanvas) {
        m_BackgroundCanvas->Draw();
    }
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

    glm::vec4 normalColor(0.7607f, 0.1607f, 0.2000f, 1.0f);
    glm::vec4 labelColor(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 disabledColor(0.4f, 0.4f, 0.4f, 1.0f);
    glm::vec4 transparentColor(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 shadowColor(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 labelShadowColor(1.0f, 1.0f, 1.0f, 0.3f);
    float shadowZ = 0.45f;

    float currentY = 200.0f;
    float elementSpacing = 120.0f;
    float buttonHeight = 75.0f;

    NFSEngine::UI::LabelParameters titleShadow;
    titleShadow.position = glm::vec3(centerX + m_ShadowOffset.x, currentY - 50.0f + m_ShadowOffset.y, shadowZ);
    titleShadow.text = "OPTIONS";
    titleShadow.font = m_Font.get();
    titleShadow.scale = 2.5f;
    titleShadow.color = shadowColor;
    NFSEngine::UI::Label(*m_Canvas, titleShadow);

    NFSEngine::UI::LabelParameters titleParams;
    titleParams.position = glm::vec3(centerX, currentY - 50.0f, 0.5f);
    titleParams.text = "OPTIONS";
    titleParams.font = m_Font.get();
    titleParams.scale = 2.5f;
    titleParams.color = normalColor;
    NFSEngine::UI::Label(*m_Canvas, titleParams);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters vsyncLabelShadow;
    vsyncLabelShadow.position = glm::vec3(leftStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    vsyncLabelShadow.text = "V-SYNC";
    vsyncLabelShadow.font = m_Font.get();
    vsyncLabelShadow.scale = m_RowScales[0];
    vsyncLabelShadow.color = labelShadowColor;
    auto& vLblShadObj = NFSEngine::UI::Label(*m_Canvas, vsyncLabelShadow);
    m_RowLabelShadows[0] = vLblShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters vsyncLabel;
    vsyncLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    vsyncLabel.text = "V-SYNC";
    vsyncLabel.font = m_Font.get();
    vsyncLabel.scale = m_RowScales[0];
    vsyncLabel.color = labelColor;
    auto& vLblObj = NFSEngine::UI::Label(*m_Canvas, vsyncLabel);
    m_RowLabelTexts[0] = vLblObj.GetComponent<NFSEngine::TextComponent>();

    bool isVsync = NFSEngine::Application::Get().GetWindow().IsVSync();
    glm::vec4 row0ValColor = isVsync ? normalColor : disabledColor;

    NFSEngine::UI::LabelParameters vsyncBtnShadow;
    vsyncBtnShadow.position = glm::vec3(rightStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    vsyncBtnShadow.text = isVsync ? "ON" : "OFF";
    vsyncBtnShadow.font = m_Font.get();
    vsyncBtnShadow.scale = m_RowScales[0];
    vsyncBtnShadow.color = shadowColor;
    auto& vBtnShadObj = NFSEngine::UI::Label(*m_Canvas, vsyncBtnShadow);
    m_RowValueShadows[0] = vBtnShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters vsyncBtn;
    vsyncBtn.position = glm::vec3(rightStart, currentY, 0.5f);
    vsyncBtn.width = 300.0f;
    vsyncBtn.height = buttonHeight;
    vsyncBtn.color = transparentColor;
    vsyncBtn.text = isVsync ? "ON" : "OFF";
    vsyncBtn.font = m_Font.get();
    vsyncBtn.textScale = m_RowScales[0];
    vsyncBtn.textColor = row0ValColor;
    vsyncBtn.onClick = [this, isVsync]() {
        NFSEngine::Application::Get().GetWindow().SetVSync(!isVsync);
        this->m_NeedsRebuild = true;
    };
    vsyncBtn.onHover = [this]() { m_FocusedRow = 0; };
    auto& vsyncBtnObj = NFSEngine::UI::Button(*m_Canvas, vsyncBtn);
    m_RowValueTexts[0] = vsyncBtnObj.GetComponent<NFSEngine::TextComponent>();

    float width0 = m_Font->GetTextWidth("V-SYNC", 1.1f);
    m_PierdolniczekPosition[0] = glm::vec2(leftStart - (width0 / 2.0f) - 70.0f, currentY);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters volLabelShadow;
    volLabelShadow.position = glm::vec3(leftStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    volLabelShadow.text = "VOLUME";
    volLabelShadow.font = m_Font.get();
    volLabelShadow.scale = m_RowScales[1];
    volLabelShadow.color = labelShadowColor;
    auto& volLblShadObj = NFSEngine::UI::Label(*m_Canvas, volLabelShadow);
    m_RowLabelShadows[1] = volLblShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters volLabel;
    volLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    volLabel.text = "VOLUME";
    volLabel.font = m_Font.get();
    volLabel.scale = m_RowScales[1];
    volLabel.color = labelColor;
    auto& volLblObj = NFSEngine::UI::Label(*m_Canvas, volLabel);
    m_RowLabelTexts[1] = volLblObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters volMinusShadow;
    volMinusShadow.position = glm::vec3(rightStart - 150.0f + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    volMinusShadow.text = "<";
    volMinusShadow.font = m_Font.get();
    volMinusShadow.scale = m_RowScales[1];
    volMinusShadow.color = shadowColor;
    auto& volMinShadObj = NFSEngine::UI::Label(*m_Canvas, volMinusShadow);
    m_RowMinusShadows[1] = volMinShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters volMinus;
    volMinus.position = glm::vec3(rightStart - 150.0f, currentY, 0.5f);
    volMinus.width = buttonHeight;
    volMinus.height = buttonHeight;
    volMinus.color = transparentColor;
    volMinus.text = "<";
    volMinus.font = m_Font.get();
    volMinus.textScale = m_RowScales[1];
    volMinus.textColor = normalColor;
    volMinus.onClick = [this]() {
        float vol = NFSEngine::AudioEngine::GetMasterVolume();
        if (vol > 0.0f) NFSEngine::AudioEngine::SetMasterVolume(vol - 0.1f);
        this->m_NeedsRebuild = true;
    };
    volMinus.onHover = [this]() { m_FocusedRow = 1; };
    auto& volMinusObj = NFSEngine::UI::Button(*m_Canvas, volMinus);
    m_RowMinusTexts[1] = volMinusObj.GetComponent<NFSEngine::TextComponent>();

    float currentVol = NFSEngine::AudioEngine::GetMasterVolume();
    std::string volString = std::to_string(static_cast<int>(std::round(currentVol * 100.0f)));

    NFSEngine::UI::LabelParameters volValShadow;
    volValShadow.position = glm::vec3(rightStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    volValShadow.text = volString;
    volValShadow.font = m_Font.get();
    volValShadow.scale = m_RowScales[1];
    volValShadow.color = shadowColor;
    auto& volValShadObj = NFSEngine::UI::Label(*m_Canvas, volValShadow);
    m_RowValueShadows[1] = volValShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters volValue;
    volValue.position = glm::vec3(rightStart, currentY, 0.5f);
    volValue.text = volString;
    volValue.font = m_Font.get();
    volValue.scale = m_RowScales[1];
    volValue.color = normalColor;
    auto& volValObj = NFSEngine::UI::Label(*m_Canvas, volValue);
    m_RowValueTexts[1] = volValObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters volPlusShadow;
    volPlusShadow.position = glm::vec3(rightStart + 150.0f + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    volPlusShadow.text = ">";
    volPlusShadow.font = m_Font.get();
    volPlusShadow.scale = m_RowScales[1];
    volPlusShadow.color = shadowColor;
    auto& volPlsShadObj = NFSEngine::UI::Label(*m_Canvas, volPlusShadow);
    m_RowPlusShadows[1] = volPlsShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters volPlus;
    volPlus.position = glm::vec3(rightStart + 150.0f, currentY, 0.5f);
    volPlus.width = buttonHeight;
    volPlus.height = buttonHeight;
    volPlus.color = transparentColor;
    volPlus.text = ">";
    volPlus.font = m_Font.get();
    volPlus.textScale = m_RowScales[1];
    volPlus.textColor = normalColor;
    volPlus.onClick = [this]() {
        float vol = NFSEngine::AudioEngine::GetMasterVolume();
        if (vol < 1.0f) NFSEngine::AudioEngine::SetMasterVolume(vol + 0.1f);
        this->m_NeedsRebuild = true;
    };
    volPlus.onHover = [this]() { m_FocusedRow = 1; };
    auto& volPlusObj = NFSEngine::UI::Button(*m_Canvas, volPlus);
    m_RowPlusTexts[1] = volPlusObj.GetComponent<NFSEngine::TextComponent>();

    float width1 = m_Font->GetTextWidth("VOLUME", 1.1f);
    m_PierdolniczekPosition[1] = glm::vec2(leftStart - (width1 / 2.0f) - 70.0f, currentY);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters fsLabelShadow;
    fsLabelShadow.position = glm::vec3(leftStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    fsLabelShadow.text = "FULLSCREEN";
    fsLabelShadow.font = m_Font.get();
    fsLabelShadow.scale = m_RowScales[2];
    fsLabelShadow.color = labelShadowColor;
    auto& fsLblShadObj = NFSEngine::UI::Label(*m_Canvas, fsLabelShadow);
    m_RowLabelShadows[2] = fsLblShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters fsLabel;
    fsLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    fsLabel.text = "FULLSCREEN";
    fsLabel.font = m_Font.get();
    fsLabel.scale = m_RowScales[2];
    fsLabel.color = labelColor;
    auto& fsLblObj = NFSEngine::UI::Label(*m_Canvas, fsLabel);
    m_RowLabelTexts[2] = fsLblObj.GetComponent<NFSEngine::TextComponent>();

    bool isFullscreen = NFSEngine::Application::Get().GetWindow().IsFullscreen();
    glm::vec4 row2ValColor = isFullscreen ? normalColor : disabledColor;

    NFSEngine::UI::LabelParameters fsBtnShadow;
    fsBtnShadow.position = glm::vec3(rightStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    fsBtnShadow.text = isFullscreen ? "ON" : "OFF";
    fsBtnShadow.font = m_Font.get();
    fsBtnShadow.scale = m_RowScales[2];
    fsBtnShadow.color = shadowColor;
    auto& fsBtnShadObj = NFSEngine::UI::Label(*m_Canvas, fsBtnShadow);
    m_RowValueShadows[2] = fsBtnShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters fsBtn;
    fsBtn.position = glm::vec3(rightStart, currentY, 0.5f);
    fsBtn.width = 300.0f;
    fsBtn.height = buttonHeight;
    fsBtn.color = transparentColor;
    fsBtn.text = isFullscreen ? "ON" : "OFF";
    fsBtn.font = m_Font.get();
    fsBtn.textScale = m_RowScales[2];
    fsBtn.textColor = row2ValColor;
    fsBtn.onClick = [this, isFullscreen]() {
        NFSEngine::Application::Get().GetWindow().SetFullscreen(!isFullscreen);
        if (isFullscreen) {
            NFSEngine::Application::Get().GetWindow().SetWindowSize(1600, 900);
            m_CurrentResIndex = 1;
        } else {
            this->SyncResolutionIndex();
        }
        this->m_NeedsRebuild = true;
    };
    fsBtn.onHover = [this]() { m_FocusedRow = 2; };
    auto& fsBtnObj = NFSEngine::UI::Button(*m_Canvas, fsBtn);
    m_RowValueTexts[2] = fsBtnObj.GetComponent<NFSEngine::TextComponent>();

    float width2 = m_Font->GetTextWidth("FULLSCREEN", 1.1f);
    m_PierdolniczekPosition[2] = glm::vec2(leftStart - (width2 / 2.0f) - 70.0f, currentY);

    currentY += elementSpacing;

    NFSEngine::UI::LabelParameters resLabelShadow;
    resLabelShadow.position = glm::vec3(leftStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    resLabelShadow.text = "RESOLUTION";
    resLabelShadow.font = m_Font.get();
    resLabelShadow.scale = m_RowScales[3];
    resLabelShadow.color = labelShadowColor;
    auto& resLblShadObj = NFSEngine::UI::Label(*m_Canvas, resLabelShadow);
    m_RowLabelShadows[3] = resLblShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters resLabel;
    resLabel.position = glm::vec3(leftStart, currentY, 0.5f);
    resLabel.text = "RESOLUTION";
    resLabel.font = m_Font.get();
    resLabel.scale = m_RowScales[3];
    resLabel.color = labelColor;
    auto& resLblObj = NFSEngine::UI::Label(*m_Canvas, resLabel);
    m_RowLabelTexts[3] = resLblObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters resMinusShadow;
    resMinusShadow.position = glm::vec3(rightStart - 280.0f + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    resMinusShadow.text = "<";
    resMinusShadow.font = m_Font.get();
    resMinusShadow.scale = m_RowScales[3];
    resMinusShadow.color = shadowColor;
    auto& resMinShadObj = NFSEngine::UI::Label(*m_Canvas, resMinusShadow);
    m_RowMinusShadows[3] = resMinShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters resMinus;
    resMinus.position = glm::vec3(rightStart - 280.0f, currentY, 0.5f);
    resMinus.width = buttonHeight;
    resMinus.height = buttonHeight;
    resMinus.color = transparentColor;
    resMinus.text = "<";
    resMinus.font = m_Font.get();
    resMinus.textScale = m_RowScales[3];
    resMinus.textColor = normalColor;
    resMinus.onClick = [this]() {
        if (m_CurrentResIndex > 0) {
            m_CurrentResIndex--;
            auto [w, h] = m_Resolutions[m_CurrentResIndex];
            NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
            this->m_NeedsRebuild = true;
        }
    };
    resMinus.onHover = [this]() { m_FocusedRow = 3; };
    auto& resMinusObj = NFSEngine::UI::Button(*m_Canvas, resMinus);
    m_RowMinusTexts[3] = resMinusObj.GetComponent<NFSEngine::TextComponent>();

    std::string resString
        = std::to_string(m_Resolutions[m_CurrentResIndex].first) + "x" + std::to_string(m_Resolutions[m_CurrentResIndex].second);

    NFSEngine::UI::LabelParameters resValShadow;
    resValShadow.position = glm::vec3(rightStart + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    resValShadow.text = resString;
    resValShadow.font = m_Font.get();
    resValShadow.scale = m_RowScales[3];
    resValShadow.color = shadowColor;
    auto& resValShadObj = NFSEngine::UI::Label(*m_Canvas, resValShadow);
    m_RowValueShadows[3] = resValShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters resValue;
    resValue.position = glm::vec3(rightStart, currentY, 0.5f);
    resValue.text = resString;
    resValue.font = m_Font.get();
    resValue.scale = m_RowScales[3];
    resValue.color = normalColor;
    auto& resValObj = NFSEngine::UI::Label(*m_Canvas, resValue);
    m_RowValueTexts[3] = resValObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::LabelParameters resPlusShadow;
    resPlusShadow.position = glm::vec3(rightStart + 280.0f + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    resPlusShadow.text = ">";
    resPlusShadow.font = m_Font.get();
    resPlusShadow.scale = m_RowScales[3];
    resPlusShadow.color = shadowColor;
    auto& resPlsShadObj = NFSEngine::UI::Label(*m_Canvas, resPlusShadow);
    m_RowPlusShadows[3] = resPlsShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters resPlus;
    resPlus.position = glm::vec3(rightStart + 280.0f, currentY, 0.5f);
    resPlus.width = buttonHeight;
    resPlus.height = buttonHeight;
    resPlus.color = transparentColor;
    resPlus.text = ">";
    resPlus.font = m_Font.get();
    resPlus.textScale = m_RowScales[3];
    resPlus.textColor = normalColor;
    resPlus.onClick = [this]() {
        if (m_CurrentResIndex < m_Resolutions.size() - 1) {
            m_CurrentResIndex++;
            auto [w, h] = m_Resolutions[m_CurrentResIndex];
            NFSEngine::Application::Get().GetWindow().SetWindowSize(w, h);
            this->m_NeedsRebuild = true;
        }
    };
    resPlus.onHover = [this]() { m_FocusedRow = 3; };
    auto& resPlusObj = NFSEngine::UI::Button(*m_Canvas, resPlus);
    m_RowPlusTexts[3] = resPlusObj.GetComponent<NFSEngine::TextComponent>();

    float width3 = m_Font->GetTextWidth("RESOLUTION", 1.1f);
    m_PierdolniczekPosition[3] = glm::vec2(leftStart - (width3 / 2.0f) - 70.0f, currentY);

    currentY += elementSpacing + 50.0f;

    NFSEngine::UI::LabelParameters backShadow;
    backShadow.position = glm::vec3(centerX + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    backShadow.text = "BACK";
    backShadow.font = m_Font.get();
    backShadow.scale = m_RowScales[4];
    backShadow.color = shadowColor;
    auto& backShadObj = NFSEngine::UI::Label(*m_Canvas, backShadow);
    m_RowLabelShadows[4] = backShadObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters backParams;
    backParams.position = glm::vec3(centerX, currentY, 0.5f);
    backParams.width = 400.0f;
    backParams.height = buttonHeight;
    backParams.color = transparentColor;
    backParams.text = "BACK";
    backParams.font = m_Font.get();
    backParams.textScale = m_RowScales[4];
    backParams.onClick = []() { GameManager::Get().CloseOptions(); };
    backParams.onHover = [this]() { m_FocusedRow = 4; };
    backParams.textColor = normalColor;
    auto& backBtnObj = NFSEngine::UI::Button(*m_Canvas, backParams);
    m_RowLabelTexts[4] = backBtnObj.GetComponent<NFSEngine::TextComponent>();

    float width4 = m_Font->GetTextWidth("BACK", 1.1f);
    m_PierdolniczekPosition[4] = glm::vec2(centerX - (width4 / 2.0f) - 70.0f, currentY);

    NFSEngine::TextureParameters ptrParams;
    ptrParams.WrapS = NFSEngine::TextureWrap::Clamp;
    ptrParams.WrapT = NFSEngine::TextureWrap::Clamp;
    ptrParams.sRGB = false;

    std::shared_ptr<NFSEngine::Texture> pointerTexture
        = NFSEngine::Texture::Create("assets/textures/ui/menu/pierdolniczek.png", ptrParams);

    NFSEngine::UI::ImageParameters indicatorParams;
    glm::vec2 pierdoPos = m_PierdolniczekPosition[m_FocusedRow];
    indicatorParams.position = glm::vec3(pierdoPos.x, pierdoPos.y, 0.6f);
    indicatorParams.width = 60.0f;
    indicatorParams.height = 90.0f;
    indicatorParams.color = { 1, 1, 1, 1 };
    indicatorParams.texture = pointerTexture;

    auto& indicatorObj = NFSEngine::UI::Image(*m_Canvas, indicatorParams);
    m_PierdolniczekTransform = &indicatorObj.Transform;
}

void OptionsLayer::SyncResolutionIndex() {
    auto& window = NFSEngine::Application::Get().GetWindow();
    uint32_t currentWidth = window.GetWidth();
    uint32_t currentHeight = window.GetHeight();

    for (int i = 0; i < m_Resolutions.size(); ++i) {
        if (m_Resolutions[i].first == currentWidth && m_Resolutions[i].second == currentHeight) {
            m_CurrentResIndex = i;
            return;
        }
    }

    int bestMatchIndex = 0;
    int minDifference = 999999;

    for (int i = 0; i < m_Resolutions.size(); ++i) {
        int diff = std::abs(m_Resolutions[i].first - static_cast<int>(currentWidth));
        if (diff < minDifference) {
            minDifference = diff;
            bestMatchIndex = i;
        }
    }

    m_CurrentResIndex = bestMatchIndex;
}