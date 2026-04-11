#pragma once

#include <functional>

#include "UI/UIComponents.hpp"

namespace NFSEngine
{

enum class ButtonState
{
    Idle,
    Hovered,
    Pressed
};

struct ButtonLogic : public UIComponent
{
    ButtonState State = ButtonState::Idle;

    glm::vec4 NormalColor = glm::vec4(1.0f);
    glm::vec4 HoverColor = glm::vec4(1.0f);
    glm::vec4 PressedColor = glm::vec4(1.0f);

    void SetColor(glm::vec4 baseColor)
    {
        NormalColor = baseColor;
        HoverColor = glm::vec4(glm::vec3(baseColor) * 0.8f, baseColor.a);
        PressedColor = glm::vec4(glm::vec3(baseColor) * 0.5f, baseColor.a);
    }

    std::function<void()> OnClick = nullptr;

    void Update();
};
}