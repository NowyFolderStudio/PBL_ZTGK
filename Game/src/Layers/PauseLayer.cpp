#include "Layers/PauseLayer.hpp"
#include "GameManager.hpp"
#include "UI/UIFactory.hpp"
#include "UI/UIRenderer.hpp"
#include "Renderer/Texture.hpp"
#include "Core/Input.hpp"

PauseLayer::PauseLayer() { m_Canvas = nullptr; }

PauseLayer::~PauseLayer() {
    if (m_Canvas) delete m_Canvas;
}

void PauseLayer::OnAttach() {
    NFSEngine::UIRenderer::Init();
    m_Canvas = new NFSEngine::Canvas();
    m_VideoDecoder = std::make_unique<NFSEngine::PLMpegDecoder>();
    if (m_VideoDecoder->OpenFile("assets/videos/pause_lq.mpg")) {
        NFSEngine::TextureParameters texParams;
        texParams.Channels = 3;
        texParams.GenerateMipmaps = false;
        texParams.WrapS = NFSEngine::TextureWrap::Clamp;
        texParams.WrapT = NFSEngine::TextureWrap::Clamp;
        texParams.sRGB = false;

        m_VideoTexture = NFSEngine::Texture::Create(m_VideoDecoder->GetWidth(), m_VideoDecoder->GetHeight(), texParams);

        if (m_VideoTexture && m_VideoDecoder->ReadNextFrame()) {
            uint32_t dataSize = m_VideoDecoder->GetDataSize();
            m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);
        }
    } else {
        NFS_CORE_ERROR("[MainMenuLayer]: Can't load background video!");
    }
    m_Font = std::make_shared<NFSEngine::Text>("assets/fonts/Super-Pandora.ttf", 72);
    BuildUI();
}

void PauseLayer::OnDetach() { }

void PauseLayer::OnUpdate(NFSEngine::DeltaTime deltaTime) {
    if (GameManager::Get().IsOptionsOpen()) return;
    if (m_Canvas) m_Canvas->Update();

    m_CreationTimer += (float)deltaTime;
    if (m_CreationTimer < 0.1f) return;

    if (m_VideoDecoder && m_VideoTexture) {
        m_VideoAccumulator += deltaTime.GetSeconds();
        float frameTime = 1.0f / static_cast<float>(m_VideoDecoder->GetFPS());

        int framesDecodedThisTick = 0;

        while (m_VideoAccumulator >= frameTime && framesDecodedThisTick < 2) {
            if (m_VideoDecoder->ReadNextFrame()) {
                uint32_t dataSize = m_VideoDecoder->GetDataSize();
                m_VideoTexture->SetData(m_VideoDecoder->GetVideoData(), dataSize);

                m_VideoAccumulator -= frameTime;
                framesDecodedThisTick++;
            } else {
                break;
            }
        }

        if (m_VideoAccumulator > frameTime * 2.0f) {
            m_VideoAccumulator = 0.0f;
        }
    }

    auto& input = NFSEngine::InputActionManager::Get();

    if (input.IsDown("UINavDown")) {
        m_FocusedIndex = (m_FocusedIndex + 1) % 3;
    } else if (input.IsDown("UINavUp")) {
        m_FocusedIndex = (m_FocusedIndex - 1 + 3) % 3;
    }

    if (input.IsDown("UIConfirm")) {
        if (m_FocusedIndex == 0)
            GameManager::Get().TogglePause();
        else if (m_FocusedIndex == 1)
            GameManager::Get().OpenOptions();
        else if (m_FocusedIndex == 2)
            GameManager::Get().RequestStateChange(GameState::MainMenu);
    }

    if (NFSEngine::Input::IsControllerButtonDown(0, NFSEngine::ControllerButtons::Start)) {
        GameManager::Get().TogglePause();
    }

    float animationSpeed = 15.0f;
    float dt = deltaTime.GetSeconds();

    for (int i = 0; i < 3; ++i) {
        float targetScale = (m_FocusedIndex == i) ? 2.0f : 1.5f;

        m_ButtonScales[i] += (targetScale - m_ButtonScales[i]) * animationSpeed * dt;

        if (m_ButtonTexts[i]) {
            m_ButtonTexts[i]->Scale = m_ButtonScales[i];
        }
        if (m_ShadowTexts[i]) {
            m_ShadowTexts[i]->Scale = m_ButtonScales[i];
        }
    }

    if (m_PierdolniczekTransform) {
        float indicatorSpeed = 15.0f;
        float currentY = m_PierdolniczekTransform->Position.y;
        float targetY = m_PierdolniczekPosition[m_FocusedIndex].y;

        float currentX = m_PierdolniczekTransform->Position.x;
        float targetX = m_PierdolniczekPosition[m_FocusedIndex].x;

        float newY = currentY + (targetY - currentY) * indicatorSpeed * dt;
        float newX = currentX + (targetX - currentX) * indicatorSpeed * dt;

        m_PierdolniczekTransform->Position.y = newY;
        m_PierdolniczekTransform->Position.x = newX;
    }
}

void PauseLayer::OnRender() {
    NFSEngine::UIRenderer::Begin();
    if (m_Canvas) {
        m_Canvas->Draw();
    }
    NFSEngine::UIRenderer::End();
}

void PauseLayer::OnEvent(NFSEngine::Event& e) {
    NFSEngine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<NFSEngine::WindowResizeEvent>([](NFSEngine::WindowResizeEvent& ev) {
        NFSEngine::Renderer::OnWindowResize(ev.GetWidth(), ev.GetHeight());
        return false;
    });

    if (e.GetEventType() == NFSEngine::EventType::KeyPressed) {
        auto& keyEvent = (NFSEngine::KeyPressedEvent&)e;
        if (keyEvent.GetKeyCode() == NFSEngine::Key::Escape) {
            GameManager::Get().TogglePause();
            e.Handled = true;
        }
    }
}

void PauseLayer::BuildUI() {
    m_Canvas->ClearUIObjects();

    const float centerX = NFSEngine::UIRenderer::VIRTUAL_WIDTH / 2.0f;
    const float centerY = NFSEngine::UIRenderer::VIRTUAL_HEIGHT / 2.0f;

    glm::vec4 normalColor(0.7607f, 0.1607f, 0.2000f, 1.0f);
    glm::vec4 transparentColor(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 shadowColor(0.0f, 0.0f, 0.0f, 1.0f);
    float shadowZ = 0.45f;

    NFSEngine::UI::ImageParameters bgParams;
    bgParams.position = glm::vec3(centerX, centerY, 0.1f);
    bgParams.width = NFSEngine::UIRenderer::VIRTUAL_WIDTH;
    bgParams.height = -NFSEngine::UIRenderer::VIRTUAL_HEIGHT;
    if (m_VideoTexture) {
        bgParams.texture = m_VideoTexture;
    } else {
        bgParams.color = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    }
    NFSEngine::UI::Image(*m_Canvas, bgParams);

    float currentY = 600.0f;
    float offsetX = -350.0f;
    float elementSpacing = 125.0f;
    float indicatorOffsetX = 250.0f;

    // ==========================================
    // 1. Resume Button
    // ==========================================
    NFSEngine::UI::LabelParameters resumeShadow;
    resumeShadow.position = glm::vec3(centerX + offsetX + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    resumeShadow.text = "Resume";
    resumeShadow.font = m_Font.get();
    resumeShadow.color = shadowColor;
    resumeShadow.scale = m_ButtonScales[0];

    auto& resumeShadowObj = NFSEngine::UI::Label(*m_Canvas, resumeShadow);
    m_ShadowTexts[0] = resumeShadowObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters resumeParams;
    resumeParams.position = glm::vec3(centerX + offsetX, currentY, 0.5f);
    resumeParams.width = 500.0f;
    resumeParams.height = 100.0f;
    resumeParams.color = transparentColor;
    resumeParams.text = "Resume";
    resumeParams.font = m_Font.get();
    resumeParams.textColor = normalColor;
    resumeParams.textScale = m_ButtonScales[0];
    resumeParams.onClick = []() { GameManager::Get().TogglePause(); };
    resumeParams.onHover = [this]() { m_FocusedIndex = 0; };

    auto& resumeBtn = NFSEngine::UI::Button(*m_Canvas, resumeParams);
    m_ButtonTexts[0] = resumeBtn.GetComponent<NFSEngine::TextComponent>();

    m_PierdolniczekPosition[0] = glm::vec2(centerX + offsetX - (m_Font->GetTextWidth("Resume", 2) / 2) - 50, currentY);

    currentY += elementSpacing;

    // ==========================================
    // 2. Options Button
    // ==========================================
    NFSEngine::UI::LabelParameters optionsShadow;
    optionsShadow.position = glm::vec3(centerX + offsetX + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    optionsShadow.text = "Options";
    optionsShadow.font = m_Font.get();
    optionsShadow.color = shadowColor;
    optionsShadow.scale = m_ButtonScales[1];

    auto& optionsShadowObj = NFSEngine::UI::Label(*m_Canvas, optionsShadow);
    m_ShadowTexts[1] = optionsShadowObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters optionsParams;
    optionsParams.position = glm::vec3(centerX + offsetX, currentY, 0.5f);
    optionsParams.width = 500.0f;
    optionsParams.height = 100.0f;
    optionsParams.color = transparentColor;
    optionsParams.text = "Options";
    optionsParams.font = m_Font.get();
    optionsParams.textColor = normalColor;
    optionsParams.textScale = m_ButtonScales[1];
    optionsParams.onClick = []() { GameManager::Get().OpenOptions(); };
    optionsParams.onHover = [this]() { m_FocusedIndex = 1; };

    auto& optionsBtn = NFSEngine::UI::Button(*m_Canvas, optionsParams);
    m_ButtonTexts[1] = optionsBtn.GetComponent<NFSEngine::TextComponent>();

    m_PierdolniczekPosition[1] = glm::vec2(centerX + offsetX - (m_Font->GetTextWidth("Options", 2) / 2) - 50, currentY);

    currentY += elementSpacing;

    // ==========================================
    // 3. Main Menu Button
    // ==========================================
    NFSEngine::UI::LabelParameters quitShadow;
    quitShadow.position = glm::vec3(centerX + offsetX + m_ShadowOffset.x, currentY + m_ShadowOffset.y, shadowZ);
    quitShadow.text = "Main Menu";
    quitShadow.font = m_Font.get();
    quitShadow.color = shadowColor;
    quitShadow.scale = m_ButtonScales[2];

    auto& quitShadowObj = NFSEngine::UI::Label(*m_Canvas, quitShadow);
    m_ShadowTexts[2] = quitShadowObj.GetComponent<NFSEngine::TextComponent>();

    NFSEngine::UI::ButtonParameters quitParams;
    quitParams.position = glm::vec3(centerX + offsetX, currentY, 0.5f);
    quitParams.width = 500.0f;
    quitParams.height = 100.0f;
    quitParams.color = transparentColor;
    quitParams.text = "Main Menu";
    quitParams.font = m_Font.get();
    quitParams.textColor = normalColor;
    quitParams.textScale = m_ButtonScales[2];
    quitParams.onClick = []() { GameManager::Get().RequestStateChange(GameState::MainMenu); };
    quitParams.onHover = [this]() { m_FocusedIndex = 2; };
    auto& quitBtn = NFSEngine::UI::Button(*m_Canvas, quitParams);
    m_ButtonTexts[2] = quitBtn.GetComponent<NFSEngine::TextComponent>();

    m_PierdolniczekPosition[2] = glm::vec2(centerX + offsetX - (m_Font->GetTextWidth("Main Menu", 2) / 2) - 50, currentY);

    // ==========================================
    // 4. Pierdolniczek
    // ==========================================
    NFSEngine::TextureParameters ptrParams;
    ptrParams.WrapS = NFSEngine::TextureWrap::Clamp;
    ptrParams.WrapT = NFSEngine::TextureWrap::Clamp;
    ptrParams.sRGB = false; // Zapobiega przyciemnianiu!

    std::shared_ptr<NFSEngine::Texture> pointerTexture
        = NFSEngine::Texture::Create("assets/textures/ui/menu/pierdolniczek.png", ptrParams);

    NFSEngine::UI::ImageParameters indicatorParams;
    glm::vec2 pierdolniczek = m_PierdolniczekPosition[m_FocusedIndex];
    indicatorParams.position = glm::vec3(pierdolniczek.x, pierdolniczek.y, 0.6f);
    indicatorParams.width = 50.0f;
    indicatorParams.height = 70.0f;
    indicatorParams.color = { 1, 1, 1, 1 };
    indicatorParams.texture = pointerTexture;

    auto& indicatorObj = NFSEngine::UI::Image(*m_Canvas, indicatorParams);
    m_PierdolniczekTransform = &indicatorObj.Transform;
}