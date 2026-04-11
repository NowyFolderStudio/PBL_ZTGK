#pragma once

#include "UI/UIComponents.hpp"
#include "UI/UIObject.hpp"
#include "UI/Canvas.hpp"
#include "UI/UIRenderer.hpp"

#include "Core/Text.hpp"
#include <functional>

namespace NFSEngine
{
namespace UI
{
    struct ImageParameters
    {
        glm::vec3 position = glm::vec3(0.0f);
        float width = 100.0f;
        float height = 100.0f;
        glm::vec4 color = glm::vec4(1.0f);
        std::shared_ptr<Texture> texture = nullptr;
    };

    struct ButtonParameters
    {
        glm::vec3 position = glm::vec3(0.0f);
        float width = 200.0f;
        float height = 100.0f;
        glm::vec4 color = glm::vec4(1.0f);
        std::shared_ptr<Texture> texture = nullptr;

        std::string text = "";
        Text* font = nullptr;
        glm::vec4 textColor = glm::vec4(0.0f);
        float textScale = 1.0f;

        std::function<void()> onClick = nullptr;
    };

    struct LabelParameters
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec4 color = glm::vec4(0.0f);
        std::string text = "";
        Text* font = nullptr;
        float scale = 1.0f;
    };

    struct CheckboxParameters
    {
        glm::vec3 position = glm::vec3(0.0f);
        float size = 20.0f;
        glm::vec4 color = glm::vec4(1.0f);
        glm::vec4 checkColor = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
        std::function<void(bool)> onToggle = nullptr;
    };

    UIObject& Image(Canvas& canvas, const ImageParameters& params);
    UIObject& Button(Canvas& canvas, const ButtonParameters& params);
    UIObject& Label(Canvas& canvas, const LabelParameters& params);
    UIObject& Checkbox(Canvas& canvas, const CheckboxParameters& params);
    // dodac slider

};
}