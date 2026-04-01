#pragma once

#include "UI/UIComponents.hpp"
#include "UI/UIObject.hpp"
#include "UI/Canvas.hpp"
#include "UI/UIRenderer.hpp"
#include <functional>

namespace NFSEngine
{
	namespace UI {

		UIObject& Image(Canvas& canvas, glm::vec3 position, float width, float height, glm::vec4 color);
		UIObject& Button(Canvas& canvas, glm::vec3 position, float width, float height, glm::vec4 color, std::function<void()> onClick);
		UIObject& Label(Canvas& canvas, glm::vec3 position, const std::string& text, Text* font = nullptr, glm::vec4 color = glm::vec4(1.0f), float scale = 1.0f);
		// dodac checkbox
		// dodac slider
		 
	};
}