#pragma once
#include "Core/DeltaTime.hpp"
#include "NFSEngine.h"
#include <memory>
#include <vector>
#include <array>
#include "UI/Canvas.hpp"
#include "Renderer/Texture.hpp"
#include "UI/UIComponents.hpp"
#include "UI/PLMpegVideoDecoder.hpp"

class OptionsLayer : public NFSEngine::Layer {
public:
    OptionsLayer();
    virtual ~OptionsLayer() override;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(NFSEngine::DeltaTime deltaTime) override;
    void OnRender() override;
    void OnEvent(NFSEngine::Event& e) override;

private:
    NFSEngine::Canvas* m_Canvas;
    int m_CurrentResIndex = 0;
    std::vector<std::pair<int, int>> m_Resolutions = { { 1280, 720 }, { 1600, 900 }, { 1920, 1080 } };

    int m_FocusedRow = 0;
    bool m_NeedsRebuild = false;

    std::shared_ptr<NFSEngine::Text> m_Font;

    std::unique_ptr<NFSEngine::PLMpegDecoder> m_VideoDecoder;
    std::shared_ptr<NFSEngine::Texture> m_VideoTexture;
    float m_VideoAccumulator = 0.0f;

    std::array<float, 5> m_RowScales = { 1.5f, 1.5f, 1.5f, 1.5f, 1.5f };
    glm::vec2 m_ShadowOffset = { 6.0f, 6.0f };

    std::array<NFSEngine::TextComponent*, 5> m_RowLabelTexts = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowValueTexts = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowLabelShadows = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowValueShadows = { nullptr, nullptr, nullptr, nullptr, nullptr };

    std::array<NFSEngine::TextComponent*, 5> m_RowMinusTexts = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowMinusShadows = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowPlusTexts = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::array<NFSEngine::TextComponent*, 5> m_RowPlusShadows = { nullptr, nullptr, nullptr, nullptr, nullptr };

    std::array<glm::vec2, 5> m_PierdolniczekPosition = { glm::vec2(), glm::vec2(), glm::vec2(), glm::vec2(), glm::vec2() };
    NFSEngine::RectTransform* m_PierdolniczekTransform = nullptr;

    void SyncResolutionIndex();
    void BuildUI();
};