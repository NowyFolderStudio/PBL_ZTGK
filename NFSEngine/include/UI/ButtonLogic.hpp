#pragma once

#include <functional>

#include "UI/UIComponents.hpp"


namespace NFSEngine {

	enum class ButtonState {
		Idle,
		Hovered,
		Pressed
	};

	struct ButtonLogic : public UIComponent {
		ButtonState State = ButtonState::Idle;

		glm::vec4 NormalColor = glm::vec4(1.0f);
		glm::vec4 HoverColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		glm::vec4 PressedColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

		std::function<void()> OnClick = nullptr;

		void Update();
	};
}