#pragma once

#include "Components/Component.hpp"
#include "Components/Transform.hpp"
#include <glm/glm.hpp>

namespace NFSEngine {
	class PointLight : public Component {
	public:
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f }; 
		float Intensity = 1.0f;

		float Constant = 1.0f;
		float Linear = 0.09f;
		float Quadratic = 0.032f;

		PointLight(GameObject* owner) : Component(owner) {}

		virtual std::string GetName() const override { return "PointLight"; }

		Transform* GetTransform() const { return m_Transform; }

		void PointLight::OnImGuiRender() override {
			ImGui::ColorEdit3("Color", glm::value_ptr(Color));
			ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);

			ImGui::Separator();
			ImGui::Text("Attenuation (Zanikanie)");

			ImGui::DragFloat("Constant", &Constant, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Linear", &Linear, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Quadratic", &Quadratic, 0.0001f, 0.0f, 1.0f);
		}

	protected:
		virtual void OnAwake() override {
			m_Transform = m_Owner->GetComponent<Transform>();
		}

	private:
		Transform* m_Transform = nullptr;
	};
}