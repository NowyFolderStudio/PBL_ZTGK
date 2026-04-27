#pragma once

#include "Components/Component.hpp"
#include <glm/glm.hpp>


namespace NFSEngine {
	class DirectionalLight : public Component {
	public:
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		float Intensity = 1.0f;

		DirectionalLight(GameObject* owner) : Component(owner) {}

		virtual std::string GetName() const override { return "DirectionalLight"; }
	};
}