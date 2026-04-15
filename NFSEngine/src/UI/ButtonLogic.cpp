#include "UI/ButtonLogic.hpp"
#include "UI/UIObject.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace NFSEngine {

    void ButtonLogic::Update() {
        if (!Owner) return;

        auto* rectTransform = &Owner->Transform;
        if (!rectTransform) return;

        float physicalWidth = (float)NFSEngine::Application::Get().GetConfig().WindowWidth;
        float physicalHeight = (float)NFSEngine::Application::Get().GetConfig().WindowHeight;

        float virtualWidth = 1920.0f;
        float virtualHeight = 1080.0f;

        float rawMouseX = NFSEngine::Input::GetMouseX();
        float rawMouseY = NFSEngine::Input::GetMouseY();

        float uiMouseX = (rawMouseX / physicalWidth) * virtualWidth;
        float uiMouseY = (rawMouseY / physicalHeight) * virtualHeight;

        float left = rectTransform->Position.x - rectTransform->Pivot.x * rectTransform->Width;
        float right = rectTransform->Position.x + (1.0f - rectTransform->Pivot.x) * rectTransform->Width;
        float top = rectTransform->Position.y - rectTransform->Pivot.y * rectTransform->Height;
        float bottom = rectTransform->Position.y + (1.0f - rectTransform->Pivot.y) * rectTransform->Height;

        bool isHovered = uiMouseX >= left && uiMouseX <= right && uiMouseY >= top && uiMouseY <= bottom;

        if (isHovered) {
            if (Input::IsMouseButtonDown(Mouse::ButtonLeft)) {
                State = ButtonState::Pressed;
            } else if (Input::IsMouseButtonUp(Mouse::ButtonLeft) && State == ButtonState::Pressed) {
                if (OnClick) {
                    OnClick();
                }
                State = ButtonState::Hovered;
            } else if (State != ButtonState::Pressed) {
                State = ButtonState::Hovered;
            }
        } else {
            State = ButtonState::Idle;
        }

        auto* image = Owner->GetComponent<ImageComponent>();
        if (image) {
            switch (State) {
            case ButtonState::Idle:
                image->Color = NormalColor;
                break;
            case ButtonState::Hovered:
                image->Color = HoverColor;
                break;
            case ButtonState::Pressed:
                image->Color = PressedColor;
                break;
            }
        }
    }

} // namespace NFSEngine