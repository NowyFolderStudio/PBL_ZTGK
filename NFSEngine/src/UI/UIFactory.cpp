#include "UI/UIComponents.hpp"
#include "UI/UIObject.hpp"
#include "UI/Canvas.hpp"
#include "UI/UIRenderer.hpp"
#include "UI/ButtonLogic.hpp"
#include "UI/UIFactory.hpp"

namespace NFSEngine {

	namespace UI {

		UIObject& Image(Canvas& canvas, glm::vec3 position, float width, float height, glm::vec4 color) {
			UIObject& obj = canvas.CreateUIObject();

			obj.Transform.Position = position;
			obj.Transform.Width = width;
			obj.Transform.Height = height;

			obj.AddComponent<ImageComponent>(color);

			return obj;
		}

		UIObject& Button(Canvas& canvas, glm::vec3 position, float width, float height, glm::vec4 color, std::function<void()> onClick) {
			UIObject& obj = canvas.CreateUIObject();

			obj.Transform.Position = position;
			obj.Transform.Width = width;
			obj.Transform.Height = height;

			obj.AddComponent<ImageComponent>(color);

			auto& logic = obj.AddComponent<ButtonLogic>();
			logic.OnClick = onClick;

			logic.NormalColor = color * logic.NormalColor;
			logic.HoverColor = color * logic.HoverColor;
			logic.PressedColor = color * logic.PressedColor;

			return obj;
		};

		UIObject& Label(Canvas& canvas, glm::vec3 position, const std::string& text, Text* font, glm::vec4 color, float scale) {
			UIObject& obj = canvas.CreateUIObject();

			obj.Transform.Position = position;

			obj.AddComponent<TextComponent>(text, font, color, scale);

			return obj;
		}
	}
}