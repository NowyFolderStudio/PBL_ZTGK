#include "UI/ButtonLogic.hpp"
#include "UI/UIObject.hpp"
#include "Core/Input.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace NFSEngine {

	void ButtonLogic::Update() {
		if (!Owner) return;

		auto* rectTransform = &Owner->Transform;
		if (!rectTransform) return;

		float mouseX = Input::GetMouseX();
		float mouseY = Input::GetMouseY();

		float left = rectTransform->Position.x - rectTransform->Pivot.x * rectTransform->Width;
		float right = rectTransform->Position.x + (1.0f - rectTransform->Pivot.x) * rectTransform->Width;
		float top = rectTransform->Position.y - rectTransform->Pivot.y * rectTransform->Height;
		float bottom = rectTransform->Position.y + (1.0f - rectTransform->Pivot.y) * rectTransform->Height;

		bool isHovered = mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom;

		if (isHovered) {
			if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
				State = ButtonState::Pressed;
			}
			else if (Input::IsMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT) && State == ButtonState::Pressed) {
				if (OnClick) {
					OnClick();
				}
				State = ButtonState::Hovered;
			}
			else if (State != ButtonState::Pressed) {
				State = ButtonState::Hovered;
			}
		}
		else {
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


}