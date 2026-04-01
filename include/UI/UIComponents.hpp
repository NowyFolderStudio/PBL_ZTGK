#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Core/Texture.hpp"

namespace NFSEngine {

	struct UIComponent {
		virtual ~UIComponent() = default;
	};

	struct RectTransform : public UIComponent {
		glm::vec3 Position = glm::vec3(0.0f);

		glm::vec2 Pivot = glm::vec2(0.5f);

		float Width = 100.0f;
		float Height = 100.0f;
		float Rotation = 0.0f;

		glm::mat4 GetTransform() const {
			glm::mat4 transform = glm::mat4(1.0f);

			transform = glm::translate(transform, Position);
			transform = glm::rotate(transform, glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::scale(transform, glm::vec3(Width, Height, 1.0f));

			transform = glm::translate(transform, glm::vec3(-Pivot.x, -Pivot.y, 0.0f));

			return transform;
		}
	};

	struct ImageComponent : public UIComponent {
		glm::vec4 Color = glm::vec4(1.0f);

		std::shared_ptr<Texture> TexturePtr = nullptr;

		ImageComponent() = default;
		ImageComponent(const glm::vec4& color) : Color(color) {}
		ImageComponent(const std::shared_ptr<Texture>& texture, const glm::vec4& tintColor = glm::vec4(1.0f)) : TexturePtr(texture), Color(tintColor) {}
	};
}