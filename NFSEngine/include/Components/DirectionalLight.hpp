#pragma once

#include "Components/Component.hpp"
#include <glm/glm.hpp>
#include <imgui.h>


namespace NFSEngine {
	class DirectionalLight : public Component {
	public:
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		float Intensity = 1.0f;

		DirectionalLight(GameObject* owner) : Component(owner) {}

		virtual std::string GetName() const override { return "DirectionalLight"; }

		void DirectionalLight::OnImGuiRender() override {
			ImGui::ColorEdit3("Color", glm::value_ptr(Color));

			ImGui::DragFloat3("Direction", glm::value_ptr(Direction), 0.01f);

			ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);
		}
	};
}