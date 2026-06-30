#pragma once

#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>
#include <array>

#include "UI/Canvas.hpp"
#include "UI/UIComponents.hpp"
#include "Core/Text.hpp"
#include "UI/PLMpegVideoDecoder.hpp"

class PauseLayer : public NFSEngine::Layer {
public:
    PauseLayer();
    virtual ~PauseLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;

    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas;

    int m_FocusedIndex = 0;
    void BuildUI();

    float m_CreationTimer = 0.0f;

    std::shared_ptr<NFSEngine::Text> m_Font;

    std::array<float, 3> m_ButtonScales = { 1.5f, 1.0f, 1.0f };
    std::array<NFSEngine::TextComponent*, 3> m_ButtonTexts = { nullptr, nullptr, nullptr };

    glm::vec2 m_ShadowOffset = { 6.0f, 6.0f };
    std::array<NFSEngine::TextComponent*, 3> m_ShadowTexts = { nullptr, nullptr, nullptr };

    std::array<glm::vec2, 3> m_PierdolniczekPosition = { glm::vec2(), glm::vec2(), glm::vec2() };
    NFSEngine::RectTransform* m_PierdolniczekTransform = nullptr;
};
