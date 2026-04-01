#pragma once

#include "UI/UIComponents.hpp"
#include "UI/UIObject.hpp"
#include "UI/Canvas.hpp"
#include "UI/UIRenderer.hpp"

namespace NFSEngine
{
	namespace UI {

		UIObject& Image(Canvas& canvas, glm::vec3 position, float width, float height, glm::vec4 color);
	};
}