#pragma once

#include "Components/Component.hpp"
#include "Components/Transform.hpp"

namespace NFSEngine {
	class SpotLight : public Component {
	public:
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Direction = { 0.0f, -1.0f, 0.0f };
		float Intensity = 1.0f;

		float CutOff = glm::cos(glm::radians(12.5f));
		float OuterCutOff = glm::cos(glm::radians(17.5f));

		float Constant = 1.0f;
		float Linear = 0.9f;
		float Quadratic = 0.032f;

		SpotLight(GameObject* owner) : Component(owner) {}

		virtual std::string GetName() const override { return "SpotLight"; }

		Transform* GetTransform() const { return m_Transform; }

		void SpotLight::OnImGuiRender() override {
            ImGui::ColorEdit3("Color", glm::value_ptr(Color));
            ImGui::DragFloat3("Direction", glm::value_ptr(Direction), 0.01f);
            ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 100.0f);

            ImGui::Separator();
            ImGui::Text("Angles (K¹ty w stopniach)");

            float innerAngle = glm::degrees(glm::acos(CutOff));
            float outerAngle = glm::degrees(glm::acos(OuterCutOff));

            bool anglesChanged = false;
            if (ImGui::DragFloat("Inner Angle", &innerAngle, 0.5f, 0.0f, 90.0f)) anglesChanged = true;
            if (ImGui::DragFloat("Outer Angle", &outerAngle, 0.5f, 0.0f, 90.0f)) anglesChanged = true;

            if (anglesChanged) {
                if (outerAngle < innerAngle) {
                    outerAngle = innerAngle;
                }

                CutOff = glm::cos(glm::radians(innerAngle));
                OuterCutOff = glm::cos(glm::radians(outerAngle));
            }

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