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

	protected:
		virtual void OnAwake() override {
			m_Transform = m_Owner->GetComponent<Transform>();
		}

	private:
		Transform* m_Transform = nullptr;
	};
}